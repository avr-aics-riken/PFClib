/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompress.cpp
 * @brief  CPfcCompressCioPod Class
 * @author aics
 */


#include "PfcCompressCioPod.h"
#include "PfcCioDfiUtil.h"
#include "PfcFunction.h"


// #################################################################
// コンストラクタ
CPfcCompressCioPod::CPfcCompressCioPod( void )
{
  m_pPod          = NULL;

  m_pDfiIN        = NULL;
  m_outDirPath    = "";
  m_compressError = D_PFC_COMPRESS_ERROR_DEFAULT;
  m_numRegion     = 0;
  m_regionHead[0] = 0.0; m_regionHead[1] = 0.0; m_regionHead[2] = 0.0;
  m_regionTail[0] = 0.0; m_regionTail[1] = 0.0; m_regionTail[2] = 0.0;
  m_numStep       = 0;
  m_optFlags      = 0;

  m_pFlowData     = NULL;
}


// #################################################################
// デストラクタ
CPfcCompressCioPod::~CPfcCompressCioPod()
{
  delete m_pPod;
  
  // delete in CPfcCompressPod
  //if( m_pFlowData != NULL ) {
  //  delete [] m_pFlowData;
  //}

}

// #################################################################
// 圧縮クラス 初期化
PFC::E_PFC_ERRORCODE
CPfcCompressCioPod::Init(
           const MPI_Comm     comm, 
           cio_DFI*     pDfiIN,
           const std::string& outDirPath,
           const double       compressError,
           const int          numRegion,
           const int          head[3],
           const int          tail[3],
           const vector<int>* pStepList,    // タイムステップリスト（対象となるタイムステップ）
                                            //   0からの連番とは限らないことに注意
           const int          optFlags
         )
{

  PFC::E_PFC_ERRORCODE ret;
  int regionID, myIDinRegion, regionMasterRankID;
  int myNumStep,myStartStepPos, regionMaxStep;
  PFC::E_PFC_DTYPE dataType;
  

  m_comm          = comm;
  m_pDfiIN        = pDfiIN;
  m_outDirPath    = outDirPath;
  m_compressError = compressError;
  m_numRegion     = numRegion;
  m_regionHead[0] = head[0]; m_regionHead[1] = head[1]; m_regionHead[2] = head[2];
  m_regionTail[0] = tail[0]; m_regionTail[1] = tail[1]; m_regionTail[2] = tail[2];
  m_numStep       = pStepList->size();
  m_pStepList     = pStepList;
  m_optFlags      = optFlags;
     
  // ランク数、ランクID取得
  MPI_Comm_size( comm, &m_numRank );
  MPI_Comm_rank( comm, &m_myRankID );

  // 属性情報の設定
  const cio_FileInfo* pDfiFileInfo = m_pDfiIN->GetcioFileInfo();
  
  std::string prefix = pDfiFileInfo->Prefix;
  
  if( pDfiFileInfo->DataType == CIO::E_CIO_FLOAT32 ) {
    dataType = PFC::E_PFC_FLOAT32;
  } else if( pDfiFileInfo->DataType == CIO::E_CIO_FLOAT64 ) {
    dataType = PFC::E_PFC_FLOAT64;
  } else {
    PFC_PRINT("### ERROR  DataType Error=%d\n",pDfiFileInfo->DataType );
    return PFC::E_PFC_ERROR;
  }

  int numComponent = pDfiFileInfo->Component;

  // 分割領域毎の情報
  ret = CPfcFunction::GetPodRegionID(
                            m_myRankID,         // (in)  ランクID
                            m_numStep,          // (in)  ステップ数
                            regionID,           // (out) 領域ID (0起点）
                            myIDinRegion,       // (out) 領域内のID (0起点）
                            regionMasterRankID  // (out) 分割領域内のマスターランクID
                        );

  // ランク毎の情報
  ret = CPfcFunction::GetPodStepInfo(
                            m_numStep,          // (in)  ステップ数
                            myIDinRegion,       // (in)  領域内のID (0起点）
                            myNumStep,      // (out) idが担当するステップ数
                            myStartStepPos,     // (out) idが開始ステップ位置
                            regionMaxStep       // (out) 領域内の各ランクが担当する最大ステップ数
                        );

  // 読み込み領域確保
  int ix_size = (m_regionTail[0]-m_regionHead[0]+1);
  int iy_size = (m_regionTail[1]-m_regionHead[1]+1);
  int iz_size = (m_regionTail[2]-m_regionHead[2]+1);

  int regionSize = numComponent*ix_size*iy_size*iz_size;

  m_pFlowData  = new double[regionSize*myNumStep];

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressCioPod::Init()\n");
  DEBUG_PRINT("   m_myRankID        =%d\n",m_myRankID);
  DEBUG_PRINT("   m_numStep         =%d\n",m_numStep);
  DEBUG_PRINT("   regionID          =%d\n", regionID);
  DEBUG_PRINT("   myIDinRegion      =%d\n", myIDinRegion);
  DEBUG_PRINT("   regionMasterRankID=%d\n", regionMasterRankID);
  DEBUG_PRINT("   myNumStep         =%d\n", myNumStep);
  DEBUG_PRINT("   myStartStepPos    =%d\n", myStartStepPos);
  DEBUG_PRINT("   regionMaxStep     =%d\n", regionMaxStep);
  DEBUG_PRINT("   numComponent      =%d\n", numComponent);
  DEBUG_PRINT("   regionSize        =%d\n", regionSize);
#endif

  // CIOファイル(sph,bovファイル）Read
#ifndef PFC_POD_USE_ORIG_ARRAYSHAPE
  //   POD圧縮データをIJKN固定として読みだす
#ifdef USE_PMLIB
  PfcPerfMon::Start(t110_c);
#endif
  ret = ReadCioFile_IJKN( 
               m_pFlowData,
               dataType, myStartStepPos, myNumStep, regionSize
            );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t110_c);
#endif
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("### ERROR  CompressCioPod ReadCioFile() Error\n" );
    return ret;
  }
#else
  ret = ReadCioFile( 
               m_pFlowData,
               dataType, myStartStepPos, myNumStep, regionSize
            );
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("### ERROR  CompressCioPod ReadCioFile() Error\n" );
    return ret;
  }
#endif

  // POD圧縮クラス（１次元）生成＆初期化
  m_pPod = new CPfcCompressPod();
    
#ifdef USE_PMLIB
  PfcPerfMon::Start(t120_c);
#endif
  ret = m_pPod->Init( 
                        m_comm,
                        m_outDirPath,
                        prefix,
                        m_compressError,
                        m_numRegion,
                        regionSize,
                        m_numStep,
                        m_optFlags,
                        m_pFlowData,
                        myNumStep
                    );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t120_c);
#endif
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("### ERROR  CompressCioPod initialize erro\n" );
    return ret;
  }

  return PFC::E_PFC_SUCCESS;
}

       
// #################################################################
// CIOファイル(sph,bovファイル)Read  original配列形状(NIJK/IJKN)順
PFC::E_PFC_ERRORCODE
CPfcCompressCioPod::ReadCioFile (
           double*   pFlowData,         // [out] [numStep][unit]
           const PFC::E_PFC_DTYPE dataType, // [in]  配列のデータタイプ
           const int startStep,         // (in)  読み込み開始ステップ (0からの連番）
           const int numStep,           // (in)  時間軸方向のサイズ
           const int unit               // (in)  領域方向のサイズ
       )
{
  CIO::E_CIO_ERRORCODE ret_cio;
  int guideCell = 0;
  unsigned int i_dummy;
  
  const cio_Domain* pCioDomain = m_pDfiIN->GetcioDomain();

  // 単精度
  if( dataType == PFC::E_PFC_FLOAT32 ) 
  {
    float   f_time;
    float   f_dummy;
    float* fVal = new float[unit];

    for ( int istep =0; istep<numStep; istep++ )
    {
      ret_cio = m_pDfiIN->ReadData(
                    fVal,
                    (*m_pStepList)[startStep+istep],
                    guideCell,
                    pCioDomain->GlobalVoxel,
                    pCioDomain->GlobalDivision,
                    m_regionHead,
                    m_regionTail,
                    f_time,
                    true,
                    i_dummy,
                    f_dummy
                );
                
      if( ret_cio != CIO::E_CIO_SUCCESS ) {
        PFC_PRINT("ERROR: DFI ReadData() ret=%d\n",ret_cio);
        return PFC::E_PFC_ERROR;
      }

      // float32では誤差が大きくなるので倍精度変換
      // float32 -> double64
      for ( int j =0; j<unit; j++ ) {
        pFlowData[unit*istep+j] = (double)fVal[j];
      }
    }
    
    delete []fVal;
  
  }
  // 倍精度
  else
  {
    double  f_time;
    double  f_dummy;

#if 0
  DEBUG_PRINT("---- CPfcCompressCioPod::ReadCioFile() \n");
  DEBUG_PRINT("   startStep=%d\n",startStep);
  DEBUG_PRINT("   numStep  =%d\n",numStep);
  DEBUG_PRINT("   unit     =%d\n",unit);
  DEBUG_PRINT("   GlobalVoxel[3]=%d %d %d\n",
                    pCioDomain->GlobalVoxel[0],
                    pCioDomain->GlobalVoxel[1],
                    pCioDomain->GlobalVoxel[2]);
  DEBUG_PRINT("   GlobalDivision[3]=%d %d %d\n",
                    pCioDomain->GlobalDivision[0],
                    pCioDomain->GlobalDivision[1],
                    pCioDomain->GlobalDivision[2]);
  DEBUG_PRINT("   m_regionHead[3]=%d %d %d\n",
                    m_regionHead[0],m_regionHead[1],m_regionHead[2]);
  DEBUG_PRINT("   m_regionTail[3]=%d %d %d\n",
                    m_regionTail[0],m_regionTail[1],m_regionTail[2]);
#endif

    for ( int istep =0; istep<numStep; istep++ )
    {
#if 0
      DEBUG_PRINT("   i=%d  step=%dt\n",i,(*m_pStepList)[startStep+i]);
#endif

      ret_cio = m_pDfiIN->ReadData(
                    &pFlowData[unit*istep],
                    (*m_pStepList)[startStep+istep],
                    guideCell,
                    pCioDomain->GlobalVoxel,
                    pCioDomain->GlobalDivision,
                    m_regionHead,
                    m_regionTail,
                    f_time,
                    true,
                    i_dummy,
                    f_dummy
                );

      if( ret_cio != CIO::E_CIO_SUCCESS ) {
        PFC_PRINT("ERROR: DFI ReadData() ret=%d\n",ret_cio);
        return PFC::E_PFC_ERROR;
      }
    }
  }

  return PFC::E_PFC_SUCCESS;
}
       
// #################################################################
// CIOファイル(sph,bovファイル)Read  配列形状(IJKN)順
PFC::E_PFC_ERRORCODE
CPfcCompressCioPod::ReadCioFile_IJKN (
           double*   pFlowData,         // [out] [numStep][unit]
           const PFC::E_PFC_DTYPE dataType, // [in]  配列のデータタイプ
           const int startStep,         // (in)  読み込み開始ステップ (0からの連番）
           const int numStep,           // (in)  時間軸方向のサイズ
           const int unit               // (in)  領域方向のサイズ
       )
{
  const cio_FileInfo* pCioFileInfo = m_pDfiIN->GetcioFileInfo();
  int   numComponent = pCioFileInfo->Component; // 成分数 

  if( pCioFileInfo->ArrayShape == CIO::E_CIO_IJKN ||
      numComponent == 1                              ) 
  {
    PFC::E_PFC_ERRORCODE ret;
#ifdef USE_PMLIB
  PfcPerfMon::Start(t112_c);
#endif
    ret =ReadCioFile (
                         pFlowData,         // [out] [numStep][unit]
                         dataType,          // [in]  配列のデータタイプ
                         startStep,         // (in)  読み込み開始ステップ (0からの連番）
                         numStep,           // (in)  時間軸方向のサイズ
                         unit               // (in)  領域方向のサイズ
          );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t112_c);
#endif
    return ret;
  }

  // 以下は
  // original 配列形状(NIJK) かつ　成分数>1 の時に
  // 配列形状(IJKN)順として読みだす処理
 
  CIO::E_CIO_ERRORCODE ret_cio;
  int guideCell = 0;
  unsigned int i_dummy;

  int nsize = unit/numComponent; // voxelサイズ
  
  const cio_Domain* pCioDomain = m_pDfiIN->GetcioDomain();

  // 単精度
  if( dataType == PFC::E_PFC_FLOAT32 ) 
  {
    float   f_time;
    float   f_dummy;
    float* fVal = new float[unit];

    for ( int istep =0; istep<numStep; istep++ )
    {
#ifdef USE_PMLIB
      PfcPerfMon::Start(t114_c);
#endif
      ret_cio = m_pDfiIN->ReadData(
                    fVal,
                    (*m_pStepList)[startStep+istep],
                    guideCell,
                    pCioDomain->GlobalVoxel,
                    pCioDomain->GlobalDivision,
                    m_regionHead,
                    m_regionTail,
                    f_time,
                    true,
                    i_dummy,
                    f_dummy
                );
#ifdef USE_PMLIB
      PfcPerfMon::Stop(t114_c);
#endif
                
      if( ret_cio != CIO::E_CIO_SUCCESS ) {
        PFC_PRINT("ERROR: DFI ReadData() ret=%d\n",ret_cio);
        return PFC::E_PFC_ERROR;
      }

      // float32では誤差が大きくなるので倍精度変換
      //   float32 -> double64
      // NIJK -> IJKN変換
      for ( int j =0; j<numComponent; j++ ) {
        for ( int k =0; k<nsize; k++ ) {
          pFlowData[unit*istep+nsize*j+k] =
                       (double)fVal[numComponent*k+j];
        }
      }
    }
    
    delete []fVal;
  
  }
  // 倍精度
  else
  {
    double  f_time;
    double  f_dummy;
    double* fVal = new double[unit];

#if 0
  DEBUG_PRINT("---- CPfcCompressCioPod::ReadCioFile() \n");
  DEBUG_PRINT("   startStep=%d\n",startStep);
  DEBUG_PRINT("   numStep  =%d\n",numStep);
  DEBUG_PRINT("   unit     =%d\n",unit);
  DEBUG_PRINT("   GlobalVoxel[3]=%d %d %d\n",
                    pCioDomain->GlobalVoxel[0],
                    pCioDomain->GlobalVoxel[1],
                    pCioDomain->GlobalVoxel[2]);
  DEBUG_PRINT("   GlobalDivision[3]=%d %d %d\n",
                    pCioDomain->GlobalDivision[0],
                    pCioDomain->GlobalDivision[1],
                    pCioDomain->GlobalDivision[2]);
  DEBUG_PRINT("   m_regionHead[3]=%d %d %d\n",
                    m_regionHead[0],m_regionHead[1],m_regionHead[2]);
  DEBUG_PRINT("   m_regionTail[3]=%d %d %d\n",
                    m_regionTail[0],m_regionTail[1],m_regionTail[2]);
#endif

    for ( int istep =0; istep<numStep; istep++ )
    {
#if 0
      DEBUG_PRINT("   i=%d  step=%dt\n",i,(*m_pStepList)[startStep+i]);
#endif

#ifdef USE_PMLIB
      PfcPerfMon::Start(t114_c);
#endif
      ret_cio = m_pDfiIN->ReadData(
                    fVal,
                    (*m_pStepList)[startStep+istep],
                    guideCell,
                    pCioDomain->GlobalVoxel,
                    pCioDomain->GlobalDivision,
                    m_regionHead,
                    m_regionTail,
                    f_time,
                    true,
                    i_dummy,
                    f_dummy
                );
#ifdef USE_PMLIB
      PfcPerfMon::Stop(t114_c);
#endif

      if( ret_cio != CIO::E_CIO_SUCCESS ) {
        PFC_PRINT("ERROR: DFI ReadData() ret=%d\n",ret_cio);
        return PFC::E_PFC_ERROR;
      }

      // float32では誤差が大きくなるので倍精度変換
      //   float32 -> double64
      // NIJK -> IJKN変換
      for ( int j =0; j<numComponent; j++ ) {
        for ( int k =0; k<nsize; k++ ) {
          pFlowData[unit*istep+nsize*j+k] =
                       fVal[numComponent*k+j];
        }
      }
    }

    delete []fVal;

  }

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// 圧縮＆ファイル出力実行
PFC::E_PFC_ERRORCODE
CPfcCompressCioPod::WriteData( void )
{
#ifdef DEBUG_PFC
  DEBUG_PRINT("CPfcCompressCioPod::WriteData() Start\n");
#endif

  // 圧縮＆POD(基底ファイル、係数ファイル）出力 
  //    ・index.pfcファイル 出力
  //    ・基底ファイル、係数ファイル 出力
#ifdef USE_PMLIB
  PfcPerfMon::Start(t310_c);
#endif
  m_pPod->WriteData();
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t310_c);
#endif

#ifdef DEBUG_PFC
  DEBUG_PRINT("CPfcCompressCioPod::WriteData() End\n");
#endif

  return PFC::E_PFC_SUCCESS;
}
