/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompress.cpp
 * @brief  CPfcCompress Class
 * @author aics
 */


#include "PfcCompress.h"
#include "PfcCioDfiUtil.h"
#include "PfcPathUtil.h"
#include "PfcFunction.h"


// #################################################################
// コンストラクタ
CPfcCompress::CPfcCompress( void )
{
  m_dfiFilePath       = "";
  m_outDirPath        = "";
  m_compressFormat    = "";
  m_compressError     = D_PFC_COMPRESS_ERROR_DEFAULT;
  m_domainDivision[0] = 0;
  m_domainDivision[1] = 0;
  m_domainDivision[2] = 0;
  m_startStep         = 0;
  m_endStep           = 0;
  m_optFlags          = 0;
  
  m_pDfiIN            = NULL;
}


// #################################################################
// デストラクタ
CPfcCompress::~CPfcCompress()
{
  if( m_pDfiIN != NULL ) {
    delete m_pDfiIN;
  }
}

// #################################################################
// 圧縮クラス 初期化
PFC::E_PFC_ERRORCODE
CPfcCompress::Init(
           const MPI_Comm     comm, 
           const std::string& dfiFilePath,
           const std::string& outDirPath,
           const std::string& compressFormat,
           const double       compressError,
           const int          domainDivision[3]
         )
{
  PFC::E_PFC_ERRORCODE ret;
  
  ret = Init( 
               comm, dfiFilePath, outDirPath,
               compressFormat, compressError, domainDivision,
               -1, -1,
               0
             );
  return ret;
}

// #################################################################
// 圧縮クラス 初期化
PFC::E_PFC_ERRORCODE
CPfcCompress::Init(
           const MPI_Comm     comm, 
           const std::string& dfiFilePath,
           const std::string& outDirPath,
           const std::string& compressFormat,
           const double       compressError,
           const int          domainDivision[3],
           const int          startStep,
           const int          endStep,
           const int          optFlags
         )
{

  CIO::E_CIO_ERRORCODE ret_cio;
  PFC::E_PFC_ERRORCODE ret;

#ifdef USE_PMLIB
  PfcPerfMon::Start(t10_c);  
#endif

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::Init() start\n");
  DEBUG_PRINT("   dfiFilePath   =%s\n",dfiFilePath.c_str());
  DEBUG_PRINT("   outDirPath    =%s\n",outDirPath.c_str());
  DEBUG_PRINT("   compressFormat=%s\n",compressFormat.c_str());
  DEBUG_PRINT("   compressError =%lf\n",compressError);
  DEBUG_PRINT("   domainDivision[3]=%d %d %d\n",
                    domainDivision[0],domainDivision[1],domainDivision[2]);
  DEBUG_PRINT("   startStep=%d\n",startStep);
  DEBUG_PRINT("   endStep  =%d\n",endStep);
  DEBUG_PRINT("   optFlags =%x\n",optFlags);
  fflush(stdout);
#endif

  m_comm              = comm;
  m_dfiFilePath       = dfiFilePath;
  m_outDirPath        = outDirPath;
  m_compressFormat    = compressFormat;
  m_compressError     = compressError;
  m_domainDivision[0] = domainDivision[0];
  m_domainDivision[1] = domainDivision[1];
  m_domainDivision[2] = domainDivision[2];
  m_startStep         = startStep;
  m_endStep           = endStep;
  m_optFlags          = optFlags;

  ret = PFC::MakeDirectory( m_outDirPath );
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("### ERROR ###\n");
    return ret;
  }
     
  // ランク数、ランクID取得
  MPI_Comm_size( comm, &m_numRank );
  MPI_Comm_rank( comm, &m_myRankID );

  // 全体ステップ数取得
  vector<int> timeStepList;
  
#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::Init() CPfcCioDfiUtil::GetDfiTimeStepList() start\n");
  fflush(stdout);
#endif

#ifdef USE_PMLIB
  PfcPerfMon::Start(t101_c);  
#endif
  ret = CPfcCioDfiUtil::GetDfiTimeStepList( m_dfiFilePath, timeStepList );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t101_c);  
#endif

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::Init() CPfcCioDfiUtil::GetDfiTimeStepList() end ret=%d\n",ret);
  DEBUG_PRINT("         timeStepList.size() = %d\n",timeStepList.size());
  fflush(stdout);
#endif
  if( ret != PFC::E_PFC_SUCCESS || timeStepList.size() == 0 ) {
    PFC_PRINT("### ERROR ###\n");
    return ret;
  }
  
  // 対象ステップ設定
  if( m_startStep == -1 ) {
    m_startStep = timeStepList[0];
  }
  if( m_endStep == -1 ) {
    m_endStep = timeStepList[ timeStepList.size()-1 ];
  }
     // 指定ステップ有無チェック
  int ist_pos  = -1;
  int iend_pos = -1;
  for( int i = 0; i<timeStepList.size(); i++ ) {
     if( m_startStep == timeStepList[i] ) {
       ist_pos = i;
     }
     if( m_endStep == timeStepList[i] ) {
       iend_pos = i;
     }
  }
  
  if( ist_pos==-1 || iend_pos==-1 ) {
    PFC_PRINT("### ERROR ###\n");
    return PFC::E_PFC_ERROR;
  }

    // 対象のみコピー
  m_stepList.clear();
  for(int i=ist_pos; i<=iend_pos; i++ ) {
    m_stepList.push_back( timeStepList[i] );
  }
  m_numStep = m_stepList.size();
  if( m_numStep < 2 ) {
    PFC_PRINT("### ERROR  number of steps is short.  m_numStep=%d\n",m_numStep );
    return PFC::E_PFC_ERROR;
  }
#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::Init() m_numStep = %d\n",m_numStep);
  fflush(stdout);
#endif

  if( m_compressFormat == "pod" )
  {
    // 時間軸方向の並列数 取得
    m_numParallel = CPfcFunction::GetPodParallel( m_numStep );
  }
  else
  {
    PFC_PRINT("### ERROR  Compress Format Error=%s\n",m_compressFormat.c_str() );
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }

  // 並列数チェック
  m_numRegion = m_domainDivision[0]*m_domainDivision[1]*m_domainDivision[2];
  if( m_numRegion*m_numParallel != m_numRank ) {
    PFC_PRINT("### ERROR  numRank=%d  numRegion=%d  numParallel=%d  numStep=%d\n",
               m_numRank,m_numRegion,m_numParallel, m_numStep );
    return PFC::E_PFC_ERROR;
  }

  // DFI Domain情報取得 for cio_DFI::ReadInit() GVoxel
  cio_Domain     domain;
  ret = CPfcCioDfiUtil::GetDfiDomain( m_dfiFilePath, domain );
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("### ERROR ###\n");
    return ret;
  }
  
#ifdef DEBUG_PFC
  DEBUG_PRINT("   m_dfiFilePath=%s\n",m_dfiFilePath.c_str());
  DEBUG_PRINT("   domain.GlobalVoxel[3]=%d %d %d\n",
                    domain.GlobalVoxel[0],domain.GlobalVoxel[1],domain.GlobalVoxel[2]);
  DEBUG_PRINT("   domain.GlobalDivision[3]=%d %d %d\n",
                    domain.GlobalDivision[0],domain.GlobalDivision[1],domain.GlobalDivision[2]);
  fflush(stdout);
#endif

  // CIO DFI 読み込みインスタンス
#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::Init() cio_DFI::ReadInit() start\n");
  fflush(stdout);
#endif
#ifdef USE_PMLIB
  PfcPerfMon::Start(t103_c);  
#endif
  m_pDfiIN = cio_DFI::ReadInit(
                        m_comm,
                        m_dfiFilePath,
                        domain.GlobalVoxel,
                        domain.GlobalDivision,
                        ret_cio );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t103_c);  
#endif
#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::Init() cio_DFI::ReadInit() end ret_cio=%d\n",ret_cio);
  fflush(stdout);
#endif
  if( ret_cio != CIO::E_CIO_SUCCESS || m_pDfiIN == NULL ) {
    PFC_PRINT("### ERROR  cio_DFI::ReadInit() dfiFilePath=%s\n",m_dfiFilePath.c_str() );
    return PFC::E_PFC_ERROR;
  }

  // 属性情報の設定
  const cio_FileInfo* pDfiFileInfo = m_pDfiIN->GetcioFileInfo();

    // ファイルフォーマット、配列形状チェック
  if( pDfiFileInfo->FileFormat == CIO::E_CIO_FMT_SPH ) {
    if( pDfiFileInfo->ArrayShape == CIO::E_CIO_NIJK ) {
    } else {
      PFC_PRINT("### ERROR  SPH file ArrayShape error. NIJK is only available.\n");
      return PFC::E_PFC_ERROR;
    }
  }
  
  m_prefix = pDfiFileInfo->Prefix;
  if( m_compressFormat == "pod" )
  {
    m_fileFormat= "pod";
  }
  else
  {
    PFC_PRINT("### ERROR  Compress Format Error=%s\n",m_compressFormat.c_str() );
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }
  
  if( pDfiFileInfo->DataType == CIO::E_CIO_FLOAT32 ) {
    m_dataType = PFC::E_PFC_FLOAT32;
  } else if( pDfiFileInfo->DataType == CIO::E_CIO_FLOAT64 ) {
    m_dataType = PFC::E_PFC_FLOAT32;
  } else {
    PFC_PRINT("### ERROR  DataType Error=%d\n",pDfiFileInfo->DataType );
    return PFC::E_PFC_ERROR;
  }
  
  if( pDfiFileInfo->ArrayShape == CIO::E_CIO_IJKN ) {
    m_arrayShape = PFC::E_PFC_IJKN;
  } else if( pDfiFileInfo->ArrayShape == CIO::E_CIO_NIJK ) {
    m_arrayShape = PFC::E_PFC_NIJK;
  } else {
    PFC_PRINT("### ERROR  ArrayShape Error=%d\n",pDfiFileInfo->ArrayShape );
    return PFC::E_PFC_ERROR;
  }
  
  m_numComponent = pDfiFileInfo->Component;
  
  int idumy = 1;
  char* cdumy = (char*)(&idumy);
  if( cdumy[0] == 0x01 ) {
    m_endianType = PFC::E_PFC_LITTLE;
  } else if ( cdumy[0] == 0x00 ) {
    m_endianType = PFC::E_PFC_BIG;
  }

  // 分割領域毎の情報
  if( m_compressFormat == "pod" )
  {
    ret = CPfcFunction::GetPodRegionID(
                            m_myRankID,     // (in)  ランクID
                            m_numStep,      // (in)  ステップ数
                            m_regionID,     // (out) 領域ID (0起点）
                            m_myIDinRegion,   // (out) 領域内のID (0起点）
                            m_regionMasterRankID  // (out) 分割領域内のマスターランクID
                        );
  }
  else
  {
    PFC_PRINT("### ERROR  Compress Format Error=%s\n",m_compressFormat.c_str() );
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }

  // 分割計算領域の開始位置&終了位置取得
  CPfcFunction::CalcHeadTail ( 
                        domain.GlobalVoxel, 
                        m_domainDivision,
                        m_regionID, 
                        m_regionHead, m_regionTail
                );
  
  m_regionSize = m_numComponent
                  * (m_regionTail[0]-m_regionHead[0]+1)
                  * (m_regionTail[1]-m_regionHead[1]+1)
                  * (m_regionTail[2]-m_regionHead[2]+1);
  
  if( m_compressFormat == "pod" )
  {
    m_pCioPod = new CPfcCompressCioPod();
    
#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::Init(): m_pCioPod->Init()\n");
  DEBUG_PRINT("   m_outDirPath=%s\n",m_outDirPath.c_str());
  DEBUG_PRINT("   m_compressError=%lf\n",m_compressError);
  DEBUG_PRINT("   m_numRegion    =%d\n", m_numRegion);
  DEBUG_PRINT("   m_regionHead[3]=%d %d %d\n",
                    m_regionHead[0],m_regionHead[1],m_regionHead[2]);
  DEBUG_PRINT("   m_regionTail[3]=%d %d %d\n",
                    m_regionTail[0],m_regionTail[1],m_regionTail[2]);
  DEBUG_PRINT("   m_stepList.size()=%d\n",m_stepList.size());
  for(int i=0; i<m_stepList.size(); i++ ) {
    DEBUG_PRINT("   m_stepList[%d]=%d\n",i,m_stepList[i]);
  }
  DEBUG_PRINT("   m_optFlags=%x\n",m_optFlags);
#endif

#ifdef USE_PMLIB
    PfcPerfMon::Start(t105_c);  
#endif
    ret = m_pCioPod->Init( m_comm,
                           m_pDfiIN,
                           m_outDirPath,
                           m_compressError,
                           m_numRegion,
                           m_regionHead,
                           m_regionTail,
                           &m_stepList,
                           m_optFlags
                    );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t105_c);  
#endif
    if( ret != PFC::E_PFC_SUCCESS ) {
      PFC_PRINT("### ERROR  CompressCioPod initialize erro\n" );
      return ret;
    }
  }
  else
  {
    PFC_PRINT("### ERROR  Compress Format Error=%s\n",m_compressFormat.c_str() );
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::Init()  end\n");
  fflush(stdout);
#endif

#ifdef USE_PMLIB
  PfcPerfMon::Stop(t10_c);  
#endif

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// 圧縮＆ファイル出力実行
PFC::E_PFC_ERRORCODE
CPfcCompress::WriteData( void )
{
#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::WriteData() start\n");
#endif
#ifdef USE_PMLIB
  PfcPerfMon::Start(t30_c);
#endif

  // 圧縮＆POD(基底ファイル、係数ファイル）出力 
  //    ・基底ファイル、係数ファイル 出力
#ifdef USE_PMLIB
  PfcPerfMon::Start(t301_c);
#endif
  m_pCioPod->WriteData();
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t301_c);
#endif

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::WriteData(): WriteIndexPfcFile() start\n");
#endif

  // index.pfcファイル 出力
#ifdef USE_PMLIB
  PfcPerfMon::Start(t302_c);
#endif
  WriteIndexPfcFile();
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t302_c);
#endif

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompress::WriteData()  end\n");
#endif

#ifdef USE_PMLIB
  PfcPerfMon::Stop(t30_c);
#endif


  return PFC::E_PFC_SUCCESS;
}
