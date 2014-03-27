/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcRestration.cpp
 * @brief  CPfcRestration Class
 * @author aics
 */

#include "PfcDefine.h"
#include "PfcPathUtil.h"
#include "PfcRestration.h"
#include "PfcTextParser.h"
#include "PfcFunction.h"


// #################################################################
// コンストラクタ
CPfcRestration::CPfcRestration( void )
{
  m_pfcFilePath       = "";

  m_bLoadCompressData = false;    // 圧縮データのメモリロードフラグ
}


// #################################################################
// デストラクタ
CPfcRestration::~CPfcRestration()
{

}

// #################################################################
// 展開クラス 初期化
PFC::E_PFC_ERRORCODE
CPfcRestration::Init(
           const std::string& pfcFilePath
         )
{
  int ierror;
#ifdef USE_PMLIB
  PfcPerfMon::Start(t10_r);
#endif
  m_pfcFilePath = pfcFilePath;
  
  /** PFCのディレクトリパスの取得 */
  std::string dirPfc = PFC::PfcPath_DirName(pfcFilePath);
  
  /** TPインスタンス */
  CPfcTextParser tpCntl;
  tpCntl.getTPinstance();

  /** TPインスタンス CPfcTimeSlice,CPfcUnitList用 */
  cio_TextParser tpCntl_cio;
  tpCntl_cio.getTPinstance();
  
  //-----------------------------------------
  // index.pfc関係 情報設定
  //-----------------------------------------
  FILE*fp = NULL;
  if( !(fp=fopen(pfcFilePath.c_str(),"rb")) ) {
    PFC_PRINT("Can't open file. (%s)\n",pfcFilePath.c_str());
    return PFC::E_PFC_ERROR_READ_INDEXFILE_OPENERROR;
  }
  fclose(fp);

  /** 入力ファイル index.pfc をセット */
  ierror = tpCntl.readTPfile(pfcFilePath);
  if ( ierror )
  {
    printf("\tinput file not found '%s'\n",pfcFilePath.c_str());
    return PFC::E_PFC_ERROR_READ_INDEXFILE_OPENERROR;
  }

#if 1
  /** 入力ファイル index.pfc をセット */
  //  CPfcTimeSlice,CPfcUnitList用
  ierror = tpCntl_cio.readTPfile(pfcFilePath);
  if ( ierror )
  {
    printf("\tinput file not found '%s'\n",pfcFilePath.c_str());
    return PFC::E_PFC_ERROR_READ_INDEXFILE_OPENERROR;
  }
#endif

  /** Fileinfoの読込み */
  if( m_fileInfo.Read(tpCntl) != PFC::E_PFC_SUCCESS ) 
  {
    PFC_PRINT("\tFileInfo Data Read error %s\n",pfcFilePath.c_str());
    return PFC::E_PFC_ERROR_READ_FILEINFO;
  }

  /** CompressInfoの読込み */
  if( m_compressInfo.Read(tpCntl) != PFC::E_PFC_SUCCESS ) 
  {
    PFC_PRINT("\tCompressInfo Data Read error %s\n",pfcFilePath.c_str());
    return PFC::E_PFC_ERROR_READ_COMPRESSINFO;
  }

  /** FilePathの読込み */
  if( m_filePath.Read(tpCntl) != PFC::E_PFC_SUCCESS )
  {
    PFC_PRINT("\tFilePath Data Read error %s\n",pfcFilePath.c_str());
    return PFC::E_PFC_ERROR_READ_FILEPATH;
  }

#if 1
  /** UnitListの読込み */
  //if( m_unitList.Read(tpCntl) != PFC::E_PFC_SUCCESS )
  if( m_unitList.Read(tpCntl_cio) != PFC::E_PFC_SUCCESS )
  {
    PFC_PRINT("\tUnitList Data Read error %s\n",pfcFilePath.c_str());
    return PFC::E_PFC_ERROR_READ_UNITLIST;
  }

  /** TimeSliceの読込み */
  //if( m_timeSlice.Read(tpCntl) != PFC::E_PFC_SUCCESS )
  if( m_timeSlice.Read(tpCntl_cio) != PFC::E_PFC_SUCCESS )
  {
    PFC_PRINT("\tTimeSlice Data Read error %s\n",pfcFilePath.c_str());
    return PFC::E_PFC_ERROR_READ_TIMESLICE;
  }
  // タイムステップリスト設定
  GetTimeStepList ( m_timeStepList );
  m_numTimeStep = m_timeStepList.size();
#endif

  /** TextParserの破棄 */
  tpCntl.remove();
#if 1
  tpCntl_cio.remove();
#endif


  //-----------------------------------------
  // proc.pfc関係 情報設定
  //-----------------------------------------

  /** proc.pfc file path の取得 */
  std::string procfile;
  if( PFC::PfcPath_isAbsolute(m_filePath.m_procPath ) ) {
    // 絶対パス
    procfile = m_filePath.m_procPath;
  } else {
    // 相対パス ： index.pfcのパスを追加
    procfile = PFC::PfcPath_ConnectPath(dirPfc,m_filePath.m_procPath);
  }

  /** proc.dfi read */
  /** TPインスタンス */
  tpCntl.getTPinstance();

  fp = NULL;
  if( !(fp=fopen(procfile.c_str(),"rb")) ) {
    PFC_PRINT("Can't open file. (%s)\n",procfile.c_str());
    return PFC::E_PFC_ERROR_READ_PROCFILE_OPENERROR;
  }
  fclose(fp);

  /** 入力ファイル proc.pfc をセット */
  ierror = tpCntl.readTPfile(procfile);
  if ( ierror )
  {
    PFC_PRINT("\tinput file not found '%s'\n",procfile.c_str());
    return PFC::E_PFC_ERROR_READ_PROCFILE_OPENERROR;
  }

  /** Domainの読込み */
  if( m_domain.Read(tpCntl) != PFC::E_PFC_SUCCESS ) 
  {
    PFC_PRINT("\tDomain Data Read error %s\n",procfile.c_str());
    return PFC::E_PFC_ERROR_READ_DOMAIN;
  }

  /** Divisionの読込み */
  if( m_division.Read(tpCntl) != PFC::E_PFC_SUCCESS )
  {
    PFC_PRINT("\tDivision Data Read error %s\n",procfile.c_str());
    return PFC::E_PFC_ERROR_READ_DIVISION;
  }

  /** TextParserの破棄 */
  tpCntl.remove();

  // FileInfo内のパスをindex.pfcパスからのパスに更新
  if( PFC::PfcPath_isAbsolute(m_fileInfo.m_dirPath ) ) {
    // 絶対パス 変更なし
  } else {
    // 相対パス：index.pfcのパスを追加
    m_fileInfo.m_dirPath = PFC::PfcPath_ConnectPath(dirPfc,m_fileInfo.m_dirPath);
  }

#ifdef USE_PMLIB
  PfcPerfMon::Stop(t10_r);
#endif

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
void
CPfcRestration::GetGlobalVoxel (
           int voxel[3]    // [out] 計算領域サイズ
       )
{
  voxel[0] = m_domain.m_globalVoxel[0];
  voxel[1] = m_domain.m_globalVoxel[1];
  voxel[2] = m_domain.m_globalVoxel[2];
}


// #################################################################
// 領域範囲(Head/Tail)計算
PFC::E_PFC_ERRORCODE
CPfcRestration::GetHeadTail (
           const int gDiv[3],   // [in]  計算空間の領域分割数
           const int regionID,  // [in]  領域ID:  0～(gDiv[0]*gDiv[1]*gDiv[2]-1)
           int head[3],         // [out] 計算領域の開始位置
           int tail[3]          // [out] 計算領域の終了位置
       )
{
  PFC::E_PFC_ERRORCODE ret;

  ret = CPfcFunction::CalcHeadTail ( 
                        m_domain.m_globalVoxel, 
                        gDiv, regionID, 
                        head, tail
                );

  return ret;
}


// #################################################################
// タイムステップリスト取得
PFC::E_PFC_ERRORCODE
CPfcRestration::GetTimeStepList (
           vector<int>& timeStepList    // [out] タイムステップリスト
       )
{
#ifdef USE_PMLIB
  PfcPerfMon::Start(t501_r);
#endif
  timeStepList.clear();
  
  for(int i=0; i<m_timeSlice.SliceList.size(); i++ ) {
    timeStepList.push_back( m_timeSlice.SliceList[i].step );
  }

#ifdef USE_PMLIB
  PfcPerfMon::Stop(t501_r);
#endif
  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 圧縮データのメモリロード チェック
PFC::E_PFC_ERRORCODE
CPfcRestration::CheckCompressDataOnMem (
           const int memUseMax,     // [in]  使用メモリMAX  単位(Mib)
           const int head[3],       // [in]  計算領域の開始位置
           const int tail[3],       // [in]  計算領域の終了位置
           double&   loadRatio      // [out] ロード可能な割合 ( 0.0 - 1.0 )
       )
{
  PFC::E_PFC_ERRORCODE ret;
  vector<int> regionIdList;
  int numRegion;

  loadRatio = -1.0;

  // 読込み対象とまる領域IDリストの生成 */
  ret = m_division.CheckReadRegion( head, tail, regionIdList);
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("error code : %d\n",(int)ret);
    return ret;
  }
  numRegion = regionIdList.size();
  
  // 対象となる領域数ループ
  int num_v_size = 0;  // total voxel_size
  for(int i=0; i<numRegion; i++) {
       int id = regionIdList[i];
       CPfcRegion* p;
       p = &(m_division.m_regionList[id]);
       num_v_size += p->m_voxelSize[0]*p->m_voxelSize[1]*p->m_voxelSize[2];
  }

  // 圧縮前のサイズ
  int num_orig_size =   num_v_size                 // ボクセルサイズ
                      * m_fileInfo.m_numComponent  // 成分数 
                      * m_numTimeStep;             // タイムステップ数
  
  // 圧縮後のサイズ
  int num_size = num_orig_size;
  if( m_compressInfo.m_compressFormat == "pod" )
  {
    for(int i=0; i<m_compressInfo.m_calculatedLayer; i++) {
      num_size /= 2;
    }
  }
  else
  {
    PFC_PRINT(" compress format error  compressFormat=%s\n",
                     m_compressInfo.m_compressFormat.c_str() );
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }
  
  int num_mb_size = 8*num_size/(1024*1024); // double size(MiB)
  
  if( num_mb_size <= memUseMax ) {
    loadRatio = 1.0;
    return PFC::E_PFC_SUCCESS;
  } else {
    loadRatio = (double)memUseMax/(double)num_mb_size;
    return PFC::E_PFC_ERROR;
  }

}


// #################################################################
// 圧縮データのメモリロード
PFC::E_PFC_ERRORCODE
CPfcRestration::LoadCompressDataOnMem(
           const int head[3],   // [in]  計算領域の開始位置
           const int tail[3]    // [in]  計算領域の終了位置
       )
{
  PFC::E_PFC_ERRORCODE ret;

  if( m_bLoadCompressData ) {
    return PFC::E_PFC_ERROR;    // ロード済み
  }
  

  m_regionIdList.clear();
  m_regionList.clear();

  // 読込み領域IDリストの生成 */
#ifdef USE_PMLIB
  PfcPerfMon::Start(t211_r);
#endif
  ret = m_division.CheckReadRegion( head, tail, m_regionIdList);
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t211_r);
#endif
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("error code : %d\n",(int)ret);
    return ret;
  }
  m_numRegion = m_regionIdList.size();

  // 対象となる領域数ループ
  for(int i=0; i<m_numRegion; i++) {

    // 領域クラスを生成する
    CPfcRestrationRegion* pRegion = new CPfcRestrationRegion;
    // 対象分割領域に登録
    m_regionList.push_back( pRegion );
    
    // 対象分割領域 初期化
    int id = m_regionIdList[i];
    
#ifdef USE_PMLIB
    PfcPerfMon::Start(t213_r);
#endif
    m_regionList[i]->Init(
                      &m_fileInfo,     // [in] File情報（属性etc.）
                      &m_compressInfo,  // [in] 圧縮情報
                      &m_timeStepList,  // [in] タイムステップリスト
                      &m_domain,            // [in] ドメイン情報
                      &(m_division.m_regionList[id])    // [in] 分割領域情報（１領域）
       );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t213_r);
#endif

    // 圧縮データロード
#ifdef USE_PMLIB
    PfcPerfMon::Start(t215_r);
#endif
    ret = m_regionList[i]->LoadCompressDataOnMem();
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t215_r);
#endif
    if( ret != PFC::E_PFC_SUCCESS ) {
      PFC_PRINT("error code : %d\n",(int)ret);
      return ret;
    }

  }
  
   // 圧縮データのメモリロードフラグ
  m_bLoadCompressData = true;
  
  m_head[0]=head[0]; m_head[1]=head[1]; m_head[2]=head[2];
  m_tail[0]=tail[0]; m_tail[1]=tail[1]; m_tail[2]=tail[2];
  
  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// ロードした圧縮データ削除
void
CPfcRestration::DeleteCompressDataOnMem( void )
{
  // 領域クラス削除
  for(int i=0; i<m_numRegion; i++ ) {
    delete m_regionList[i];
  }
  m_numRegion = 0;
  m_regionIdList.clear();
  m_regionList.clear();

  // 圧縮データのメモリロードフラグ
  m_bLoadCompressData = false;
}

// #################################################################
// データ読み込み（範囲指定）
//    LoadCompressDataOnMem()が実行されている場合、メモリから展開する
PFC::E_PFC_ERRORCODE
CPfcRestration::ReadData (
              double*  v,           // [out] 展開後の出力領域 
                                    //       メモリは確保済みであること
                                    //       head-tail間のボクセルサイズ*成分数
              const int step,       // [in]  ステップ番号
              const int head[3],    // [in]  計算領域の開始位置
              const int tail[3]     // [in]  計算領域の終了位置
           )
{
  PFC::E_PFC_ERRORCODE ret;
  bool bNewLoad = false;
#ifdef USE_PMLIB
  PfcPerfMon::Start(t20_r);
#endif

  // 圧縮データ ロード済み
  if( m_bLoadCompressData )
  {
    // head/tailが同一サイズかどうかチェック
    if( m_head[0]==head[0] && m_head[1]==head[1] && m_head[2]==head[2] && 
        m_tail[0]==tail[0] && m_tail[1]==tail[1] && m_tail[2]==tail[2]    ) {
    } else {
      return PFC::E_PFC_ERROR;
    }
  }
  // 圧縮データ ロードなし
  else
  {
    // 圧縮データ ロード
#ifdef USE_PMLIB
    PfcPerfMon::Start(t201_r);
#endif
    ret = LoadCompressDataOnMem( head, tail );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t201_r);
#endif
    if( ret != PFC::E_PFC_SUCCESS ) {
      PFC_PRINT("Error at LoadCompressDataOnMem\n");
      return ret;
    }
    
    bNewLoad = true;
  }
  
  //--------------------------------------------------
  //  領域Read
  //--------------------------------------------------
  // 対象となる領域数ループ
  for(int i=0; i<m_regionIdList.size(); i++) {
    int out_offset_st[3], out_size  [3];   
    
    // 分割領域クラスよりをRead （部分的に値が設定されることに注意）
#ifdef USE_PMLIB
    PfcPerfMon::Start(t202_r);
#endif
    ret = m_regionList[i]->ReadDataInRange (
                    v,              // [out] 展開後の出力領域 
                    step,           // [in]  ステップ番号
                    head,           // [in]  対象計算領域の開始位置
                    tail,           // [in]  対象計算領域の終了位置
                    out_offset_st,  // [out] vの更新領域のheadからのオフセット
                    out_size        // [out] vの更新領域のサイズ
           );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t202_r);
#endif
    if( ret != PFC::E_PFC_SUCCESS ) {
      PFC_PRINT("Error ReadDataInRange\n");
      return ret;
    }
  }

  // 新規に圧縮した場合、圧縮データ 削除
  if( bNewLoad ) {
    DeleteCompressDataOnMem();
  }

#ifdef USE_PMLIB
  PfcPerfMon::Stop(t20_r);
#endif

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// データ読み込み（位置指定）
//    LoadCompressDataOnMem()が実行されている場合、メモリから展開する
PFC::E_PFC_ERRORCODE
CPfcRestration::ReadData (
              double*  v,           // [out] 展開後の出力領域 
                                    //       メモリは確保済みであること [成分数]
              const int step,       // [in]  ステップ番号
              const int ix,         // [in]  x position  ( >= 1 )  headの開始は1からのため、それに合わせる
              const int iy,         // [in]  y position  ( >= 1 )  headの開始は1からのため、それに合わせる
              const int iz          // [in]  z position  ( >= 1 )  headの開始は1からのため、それに合わせる
           )
{
  PFC::E_PFC_ERRORCODE ret;
  int regionID;
  CPfcRestrationRegion* pRegion=NULL;

  // 対象となる領域IDを求める
  ret = m_division.CheckReadRegion( ix, iy, iz, regionID);
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("Out of Range\n");
    return PFC::E_PFC_ERROR_OUT_OF_RANGE;
  }

  // 圧縮データ ロード済み
  if( m_bLoadCompressData )
  {
    
    // ロード済みのポインタ設定
    for(int i=0; i<m_regionIdList.size(); i++) {
      if( m_regionIdList[i] == regionID ) {
        pRegion = m_regionList[i];
        break;
      }
    }
    
    if( pRegion == NULL ) {
        PFC_PRINT("Out of Range on Mem\n");
      return PFC::E_PFC_ERROR_OUT_OF_RANGE;
    }
  }
  // 圧縮データ ロードなし
  else
  {
    pRegion = new CPfcRestrationRegion;
    
    pRegion->Init(
                      &m_fileInfo,     // [in] File情報（属性etc.）
                      &m_compressInfo,  // [in] 圧縮情報
                      &m_timeStepList,  // [in] タイムステップリスト
                      &m_domain,            // [in] ドメイン情報
                      &(m_division.m_regionList[regionID])  // [in] 分割領域情報（１領域）
       );
   }

  //--------------------------------------------------
  //  値Read
  //--------------------------------------------------

  ret = pRegion->ReadData (
                    v,      // [out] 展開後の出力値（１要素） 
                    step,   // [in]  ステップ番号
                    ix,     // [in]  x position
                    iy,     // [in]  y position
                    iz      // [in]  z position
           );
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("Error ReadData\n");
    return ret;
  }

  // 圧縮データ ロード済みでない場合、領域削除
  if( !m_bLoadCompressData )  {
    delete pRegion;
  }

  return PFC::E_PFC_SUCCESS;
}
