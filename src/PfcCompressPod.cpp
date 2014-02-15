/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompressPod.cpp
 * @brief  CPfcCompressPod Class
 * @author aics
 */


#include "PfcCompressPod.h"
#include "PfcFunction.h"


// #################################################################
// コンストラクタ
CPfcCompressPod::CPfcCompressPod( void )
{
  m_outDirPath        = "";
  m_prefix            = "";
  m_compressError     = D_PFC_COMPRESS_ERROR_DEFAULT;
  m_numRegion         = 0;
  m_regionSize        = 0;
  m_numStep           = 0;
  m_optFlags          = 0;

  m_pFlowData         = NULL;
  m_myNumStep         = 0;
  m_layerNo           = 0;
  m_curPodBaseSize    = 0;
  m_curNumSize        = 0;
  m_curEvaluateError  = 0.0;  // カレントのレイヤー積算誤差率(%)
  m_pPod_base_r       = NULL;
  m_pCoef_a           = NULL;
}


// #################################################################
// デストラクタ
CPfcCompressPod::~CPfcCompressPod()
{
  if( m_pFlowData != NULL )   delete [] m_pFlowData;

  if( m_pPod_base_r != NULL ) delete [] m_pPod_base_r;

  if( m_pCoef_a != NULL )     delete [] m_pCoef_a;
}



// #################################################################
// POD圧縮クラス 初期化
PFC::E_PFC_ERRORCODE
CPfcCompressPod::Init(
           const MPI_Comm     comm, 
           const std::string& outDirPath,       // [in] フィールドデータの出力ディレクトリパス
           const std::string& prefix,           // [in] ベースファイル名  （属性名）
           const double       compressError,    // [in] 誤差率(%)  ユーザ指定誤差
           const int          numRegion,        // [in] 分割領域数
           const int          regionSize,       // [in] 分割領域サイズ（成分数含む）
           const int          numStep,          // [in] タイムステップ数
           const int          optFlags,         // [in] オプション
           double*            pFlowData,        // [in] 圧縮するデータ領域
                                                //        サイズ：regionSize*myNumStep
           const int          myNumStep         // [in] 自身が担当するステップ数
         )
{

  PFC::E_PFC_ERRORCODE ret;
  int myNumStep_wk;
  
  //---------------------------------------
  m_comm              = comm;
  m_outDirPath        = outDirPath;
  m_prefix            = prefix;         // ベースファイル名  （属性名）
  m_compressError     = compressError;
  m_numRegion         = numRegion;
  m_regionSize        = regionSize;
  m_numStep           = numStep;
  m_optFlags          = optFlags;
  m_pFlowData         = pFlowData;
  m_myNumStep         = myNumStep;
     
  // ランク数、ランクID取得
  MPI_Comm_size( comm, &m_numRank );
  MPI_Comm_rank( comm, &m_myRankID );

  // 時間軸方向の並列数 取得
  m_numParallel = CPfcFunction::GetPodParallel( m_numStep );

  // 並列数チェック
  if( m_numRegion*m_numParallel != m_numRank ) {
    PFC_PRINT("### ERROR  numRank=%d  numRegion=%d  numParallel  numStep=%d\n",
               m_numRank,m_numRegion,m_numParallel, m_numStep );
    return PFC::E_PFC_ERROR;
  }

  // 全体としての情報
  m_maxLayer = CPfcFunction::GetPodMaxLayer( m_numStep );
  m_calculatedLayer = 0;


  // 分割領域毎の情報
  ret = CPfcFunction::GetPodRegionID(
                            m_myRankID,     // (in)  ランクID
                            m_numStep,      // (in)  ステップ数
                            m_regionID,     // (out) 領域ID (0起点）
                            m_myIDinRegion, // (out) 領域内のID (0起点）
                            m_regionMasterRankID  // (out) 分割領域内のマスターランクID
                        );
  if( ret !=  PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("ERROR GetPodRegionID() ret=%d\n",ret);
    return PFC::E_PFC_ERROR;
  }

  // ランク毎の情報
  ret = CPfcFunction::GetPodStepInfo(
                            m_numStep,          // (in)  ステップ数
                            m_myIDinRegion,     // (in)  領域内のID (0起点）
                            myNumStep_wk,       // (out) idが担当するステップ数
                            m_myStartStepPos,   // (out) idが開始ステップ位置
                            m_regionMaxStep     // (out) 領域内の各ランクが担当する最大ステップ数
                        );
  if( ret !=  PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("ERROR GetPodRegionID() ret=%d\n",ret);
    return PFC::E_PFC_ERROR;
  }

  if( myNumStep_wk !=  m_myNumStep ) {
    PFC_PRINT("### ERROR  myNumStep_wk=%d  m_myNumStep=%d\n",myNumStep_wk, m_myNumStep);
    return PFC::E_PFC_ERROR;
  }
  
  // 実行時のカレント情報
  m_layerNo   = -1;


  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// 圧縮実行
PFC::E_PFC_ERRORCODE
CPfcCompressPod::WriteData( void )
{
  PFC::E_PFC_ERRORCODE ret = PFC::E_PFC_SUCCESS;
  m_pPod_base_r = NULL; // 基底データ
  m_pCoef_a     = NULL; // 係数データ

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressPod::WriteData()  Start\n");
  DEBUG_PRINT("   m_maxLayer      = %d\n",m_maxLayer);
  DEBUG_PRINT("   m_myNumStep     = %d\n",m_myNumStep);
  DEBUG_PRINT("   m_regionMaxStep = %d\n",m_regionMaxStep);
#endif

  // 読み込み領域確保
  m_curPodBaseSize = m_regionSize;
  m_pCoef_a        = new double[m_regionMaxStep*m_maxLayer]; // 最大でアロケート

  // MAX レイヤーまでループ
  int finish_flg = 0;
  m_layerNo = 0;

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressPod::WriteData()  Calculation Start\n");
#endif
  
  while( m_layerNo < m_maxLayer )   // MAX レイヤーまでループ
  {
    if( m_layerNo == 0 ) {
      m_curNumSize = m_myNumStep;   // 対象とするタイムステップ数を設定
    } else {
      m_curNumSize = 2;
    }

#ifdef DEBUG_PFC
    DEBUG_PRINT("  --- layerNo=%d  curNumSize=%d -----\n",m_layerNo,m_curNumSize);
#endif
    
    // pod_base_r領域確保
    m_pPod_base_r = new double[m_curPodBaseSize];
    
    // 基底計算
#ifdef DEBUG_PFC
    DEBUG_PRINT("     CalcPodBase()  Start\n");
#endif
#ifdef USE_PMLIB
    PfcPerfMon::Start(t312_c);
#endif
    ret = CalcPodBase(
                        m_pFlowData,        // (in)
                        m_curPodBaseSize,   // (in)
                        m_curNumSize,       // (in)
                        m_pPod_base_r       // (out)
                    );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t312_c);
#endif
#ifdef DEBUG_PFC
    DEBUG_PRINT("     CalcPodBase()  End  ret=%d\n",ret);
#endif
    if( ret != PFC::E_PFC_SUCCESS ) {
      break;
    }
      
 
    // 係数計算
#ifdef DEBUG_PFC
    DEBUG_PRINT("     CalcPodCoef()  Start\n");
#endif
#ifdef USE_PMLIB
    PfcPerfMon::Start(t314_c);
#endif
    CalcPodCoef(
                m_pPod_base_r,      // (in)
                m_pFlowData,        // (in)
                m_curPodBaseSize,   // (in)
                m_curNumSize,       // (in)
                &m_pCoef_a[m_regionMaxStep*m_layerNo]   // (out)
            );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t314_c);
#endif
#ifdef DEBUG_PFC
    DEBUG_PRINT("     CalcPodCoef()  End\n");
#endif
    
    // 終了判定
    //   以下の条件の時に終了したとみなす
    //   ・m_maxLayerに到達した
    //   ・pod_base_sizeが分割できなくなった
    //   ・誤差が指定値を超えた
#ifdef DEBUG_PFC
    DEBUG_PRINT("     CheckFinish()  Start\n");
#endif
#ifdef USE_PMLIB
    PfcPerfMon::Start(t316_c);
#endif
    ret = CheckFinish (
                m_layerNo,          // (in)
                m_curPodBaseSize,   // (in)
                m_curNumSize,       // (in)
                m_regionMaxStep,    // (in)
                m_compressError,    // (in)
                m_curEvaluateError, // (in/out)
                m_pFlowData,        // (in)
                m_pPod_base_r,      // (in)
                m_pCoef_a,          // (in)
                finish_flg          // (out)
            );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t316_c);
#endif
#ifdef DEBUG_PFC
    DEBUG_PRINT("     CheckFinish()  End  ret=%d\n",ret);
#endif
    if( ret != PFC::E_PFC_SUCCESS ) {
      break;
    }
    
    // 圧縮終了
    if( finish_flg == 1 ) { // 圧縮終了
      m_calculatedLayer = m_layerNo + 1;
#ifdef DEBUG_PFC
      DEBUG_PRINT("---- CPfcCompressPod::WriteData()  End of Compression\n");
      fflush(stdout);
#endif
      break;    //**** ファイル出力・終了化処理へ *******
    }

#if 1
    // デバッグ用基底データ出力
    //     途中のレイヤーの基底データ出力
    //     最終レイヤーは別途出力するので、ここでは出力しない
    
    if( PFC_CHECK_OPT_SAVE(m_optFlags) )
    {
#ifdef DEBUG_PFC
      DEBUG_PRINT("---- CPfcCompressPod::WriteData()  WritePodBaseFile_Debug() Start\n");
      fflush(stdout);
#endif
      m_calculatedLayer = m_layerNo + 1;
      ret = WritePodBaseFile_Debug( m_pPod_base_r );
#ifdef DEBUG_PFC
      DEBUG_PRINT("---- CPfcCompressPod::WriteData()  WritePodBaseFile_Debug() End ret=%d\n",ret);
      fflush(stdout);
#endif
    }
#endif
    
    // バイナリスワップ
    delete [] m_pFlowData; m_pFlowData=NULL;
   
#ifdef DEBUG_PFC
    DEBUG_PRINT("     SwapBinaryData()  Start\n");
#endif
#ifdef USE_PMLIB
    PfcPerfMon::Start(t318_c);
#endif
    ret = SwapBinaryData( 
                    m_pPod_base_r,      // (in)
                    m_curPodBaseSize,   // (in/out)
                    m_pFlowData         // (out) pointer out
                ); 
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t318_c);
#endif
#ifdef DEBUG_PFC
    DEBUG_PRINT("     SwapBinaryData()  End  ret=%d\n",ret);
#endif

    delete [] m_pPod_base_r; m_pPod_base_r=NULL;
    
    m_layerNo++;    // レイヤー数 カントアップ
  }

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressPod::WriteData()  Calculation End\n");
  DEBUG_PRINT("        m_calculatedLayer = %d\n",m_calculatedLayer);
  fflush(stdout);
#endif

  // ファイル出力 
  //    ・基底ファイル、係数ファイル 出力
  //    ・index.pfcファイル 出力
  if( ret == PFC::E_PFC_SUCCESS ) {
#ifdef DEBUG_PFC
    DEBUG_PRINT("---- CPfcCompressPod::WriteData() Output() Start\n");
    fflush(stdout);
#endif

#ifdef USE_PMLIB
    PfcPerfMon::Start(t330_c);
#endif
    Output();
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t330_c);
#endif

#ifdef DEBUG_PFC
    DEBUG_PRINT("---- CPfcCompressPod::WriteData() Output() End\n");
    fflush(stdout);
#endif
  }

  // 終了化処理
  if( m_pFlowData != NULL ) {
    delete [] m_pFlowData;  m_pFlowData=NULL;
  }

  if( m_pCoef_a != NULL ) {
    delete [] m_pCoef_a;  m_pCoef_a=NULL;
  }

  if( m_pPod_base_r != NULL ) {
    delete [] m_pPod_base_r;  m_pPod_base_r=NULL;
  }

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressPod::WriteData()  End\n");
#endif

  return ret;
}

// #################################################################
// POD 計算レイヤー数取得
int
CPfcCompressPod::GetCalculatedLayer( void )
{
  return m_calculatedLayer;
}
