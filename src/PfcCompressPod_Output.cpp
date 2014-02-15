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
#include "PfcMpiCom.h"
#include "PfcPodFile.h"


// #################################################################
// 出力処理
PFC::E_PFC_ERRORCODE
CPfcCompressPod::Output( void )
{
  PFC::E_PFC_ERRORCODE ret;

  //   PODファイル出力  （基底ファイル）
#ifdef USE_PMLIB
  PfcPerfMon::Start(t332_c);
#endif
  ret = WritePodBaseFile( m_pPod_base_r );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t332_c);
#endif
  if( ret !=PFC::E_PFC_SUCCESS ) {
    return ret;
  }

  //   PODファイル出力  （係数ファイル）
#ifdef USE_PMLIB
  PfcPerfMon::Start(t334_c);
#endif
  ret = WritePodCoefFile( m_pCoef_a );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t334_c);
#endif
  if( ret !=PFC::E_PFC_SUCCESS ) {
    return ret;
  }

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// POD 基底ファイル 出力
PFC::E_PFC_ERRORCODE
CPfcCompressPod::WritePodBaseFile(
                    double*   pPod_base_r     // (in) 基底データ
                                                    //          サイズ：m_curPodBaseSize
                )
{
  PFC::E_PFC_ERRORCODE  ret = PFC::E_PFC_SUCCESS;

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressPod::WritePodBaseFile()  Start\n");
  fflush(stdout);
#endif

  //--------------------------------------------
  //   領域内の基底データの収集
  //--------------------------------------------
  int max_size_pod_base;

    // 領域内のm_curPodBaseSize取得
#ifdef DEBUG_PFC
  DEBUG_PRINT("   CPfcMpiCom::GetMaxInt()  Start\n");
  fflush(stdout);
#endif
#ifdef USE_PMLIB
  PfcPerfMon::Start(t341_c);
#endif
  CPfcMpiCom::GetMaxInt( 
                 m_comm, m_regionMasterRankID, m_numParallel, m_myRankID,
                 m_curPodBaseSize,max_size_pod_base
               );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t341_c);
#endif

  double* pPod_base_in_region;
  if( m_myRankID == m_regionMasterRankID ) {
    int nalloc_pod_base_in_region = max_size_pod_base*m_numParallel; // 余裕あり
    pPod_base_in_region  = new double[nalloc_pod_base_in_region];
  } else {
    DEBUG_PRINT("   pPod_base_in_region is NULL setted\n");
    pPod_base_in_region = NULL;
  }
  int*    nsize_pod_base_ranks = new int[m_numParallel];

    // 領域内の基底データ収集
#ifdef DEBUG_PFC
  DEBUG_PRINT("   CPfcMpiCom::GatherV_DataDouble()  Start\n");
  DEBUG_PRINT("     m_regionMasterRankID=%d\n",m_regionMasterRankID);
  DEBUG_PRINT("     m_numParallel       =%d\n",m_numParallel);
  DEBUG_PRINT("     m_myRankID          =%d\n",m_myRankID);
  DEBUG_PRINT("     m_curPodBaseSize    =%d\n",m_curPodBaseSize);
  DEBUG_PRINT("     max_size_pod_base   =%d\n",max_size_pod_base);
  fflush(stdout);
#endif
#ifdef USE_PMLIB
  PfcPerfMon::Start(t343_c);
#endif
  CPfcMpiCom::GatherV_DataDouble(
               m_comm, m_regionMasterRankID, m_numParallel, m_myRankID,
               m_curPodBaseSize, pPod_base_r,
               pPod_base_in_region, nsize_pod_base_ranks
            );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t343_c);
#endif

  //--------------------------------------------
  //   基底ファイル出力
  //--------------------------------------------
  
  //***** マスターランクIDのみファイル出力 *******
  if( m_myRankID == m_regionMasterRankID )
  {
    bool bSingle;
    if( m_numRegion == 1 ) {
      bSingle = true;
    } else {
      bSingle = false;
    }

  //--- 基底ファイル出力 -----------
#ifdef DEBUG_PFC
    DEBUG_PRINT("   CPfcPodFile::WriteBaseFile()  Start\n");
#endif
#ifdef USE_PMLIB
    PfcPerfMon::Start(t345_c);
#endif
    ret = CPfcPodFile::WriteBaseFile(
                m_outDirPath,           // (in) 出力ディレクトリ
                m_prefix,               // (in) 属性名
                m_regionID,             // (in) 領域ID
                bSingle,              // (in) 単一領域フラグ
                m_numStep,              // (in) タイムステップ数
                m_numParallel,          // (in) 圧縮時並列数
                m_calculatedLayer,      // (in) 圧縮時計算レイヤー数
                nsize_pod_base_ranks,   // (in) 各ランク内の要素数
                pPod_base_in_region     // (in) 基底データ
            );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t345_c);
#endif

  }
  
  if( pPod_base_in_region != NULL  ) {
#ifdef DEBUG_PFC
    DEBUG_PRINT("   delete start: pPod_base_in_region\n");
#endif

    delete [] pPod_base_in_region;

#ifdef DEBUG_PFC
    DEBUG_PRINT("   delete end  : pPod_base_in_region\n");
#endif
  }

  if( nsize_pod_base_ranks != NULL ) {
#ifdef DEBUG_PFC
    DEBUG_PRINT("   delete start: nsize_pod_base_ranks\n");
#endif

    delete [] nsize_pod_base_ranks;

#ifdef DEBUG_PFC
    DEBUG_PRINT("   delete start: nsize_pod_base_ranks\n");
#endif
  }

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressPod::WritePodBaseFile()  End\n");
#endif

  return ret;
}


// #################################################################
// POD 基底ファイル 出力 （デバッグ出力）
//   途中のレイヤーの出力：各ランクで別々に出力する
PFC::E_PFC_ERRORCODE
CPfcCompressPod::WritePodBaseFile_Debug(
                    double*   pPod_base_r     // (in) 基底データ
                                              //          サイズ：m_curPodBaseSize
                )
{
  PFC::E_PFC_ERRORCODE  ret = PFC::E_PFC_SUCCESS;

  ret = CPfcPodFile::WriteBaseFile_Debug(
                m_outDirPath,         // (in) 出力ディレクトリ
                m_prefix,             // (in) 属性名
                m_regionID,           // (in) 領域ID
                m_myIDinRegion,        // (in) 領域内のID
                m_layerNo,            // (in) レイヤーNo （0起点）
                m_numStep,            // (in) タイムステップ数
                m_numParallel,        // (in) 圧縮時並列数
                m_calculatedLayer,    // (in) 圧縮時計算レイヤー数
                                      //       layerNo:numCalculatedLayer-1
                m_curPodBaseSize,     // (in) 各ランクの要素数
                pPod_base_r           // (in) 各ランクの基底データ
            );

  return ret;
}


// #################################################################
// POD 係数ファイル 出力
PFC::E_PFC_ERRORCODE
CPfcCompressPod::WritePodCoefFile(
                    double*   pCoef_a         // (in) 係数データ
                                                    //          サイズ：[m_regionMaxStep][m_calculatedLayer]
                )
{
  PFC::E_PFC_ERRORCODE  ret = PFC::E_PFC_SUCCESS;

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressPod::WritePodCoefFile()  Start\n");
  DEBUG_PRINT("      m_regionMaxStep  =%d\n",m_regionMaxStep);
  DEBUG_PRINT("      m_calculatedLayer=%d\n",m_calculatedLayer);
  DEBUG_PRINT("      m_numParallel    =%d\n",m_numParallel);
#endif

  //--------------------------------------------
  //   係数データの収集
  //--------------------------------------------
  
  // 領域内係数データ アロケート
  int nalloc_coef_in_region = m_regionMaxStep*m_calculatedLayer*m_numParallel;
  double* pCoef_a_in_region = new double[nalloc_coef_in_region]; // [m_numParallel][m_calculatedLayer][m_regionMaxStep]
  int*    nstep_coef_ranks = new int[m_numParallel];  // 各ランクのm_myNumStepを集めたもの

  // 領域内の係数データ収集
  CPfcMpiCom::GatherDataDouble( 
               m_comm, m_regionMasterRankID, m_numParallel, m_myRankID,
               m_regionMaxStep*m_calculatedLayer, pCoef_a,
               pCoef_a_in_region
            );

  // 領域内の係数データ収集（担当ステップ数）
  CPfcMpiCom::GatherDataInt( 
               m_comm, m_regionMasterRankID, m_numParallel, m_myRankID,
               1, &m_myNumStep,
               nstep_coef_ranks
            );

  //***** マスターランクIDのみファイル出力 *******
  if( m_myRankID == m_regionMasterRankID )
  {
#ifdef DEBUG_PFC
    for(int i=0; i<m_numParallel; i++ ) {
      DEBUG_PRINT("    nstep_coef_ranks[%d]=%d\n\n",i,nstep_coef_ranks[i]);
    }
#endif

    //--- 係数データをレイヤ毎に並び替える ------
    int nalloc_coef_layer = m_numStep*m_calculatedLayer;
    double* pCoef_a_layer = new double[nalloc_coef_layer]; // [m_calculatedLayer][m_numStep];
    int ip = 0;
    for(int j=0; j<m_numParallel; j++ ) {
      for(int k=0; k<nstep_coef_ranks[j]; k++ ) {
        int kp = m_calculatedLayer*m_regionMaxStep*j + k;
        pCoef_a_layer[ip] = pCoef_a_in_region[kp];
        ip++;
      }
    }
     // レイヤー1以降
    for(int i=1; i<m_calculatedLayer; i++ ) {
      for(int j=0; j<m_numParallel; j++ ) {
        for(int k=0; k<2; k++ ) {   // 1並列あたり係数は２個対応
          int kp = m_calculatedLayer*m_regionMaxStep*j + m_regionMaxStep*i + k;
          pCoef_a_layer[m_numStep*i+2*j+k] = pCoef_a_in_region[kp];
        }
      }
    }

    //--------------------------------------------
    //   係数ファイル出力
    //--------------------------------------------
  
    int numCoef = m_numParallel*2;
    bool bSingle;
    if( m_numRegion == 1 ) {
      bSingle = true;
    } else {
      bSingle = false;
    }

    //--- 係数ファイル出力 -----------
    ret = CPfcPodFile::WriteCoefFile(
                m_outDirPath,           // (in) 出力ディレクトリ
                m_prefix,               // (in) 属性名
                m_regionID,             // (in) 領域ID
                bSingle,                // (in) 単一領域フラグ
                m_numStep,              // (in) タイムステップ数
                                        //          == Layer0の係数出力数
                numCoef,                // (in) 係数出力数（Layer1以降）
                m_calculatedLayer,      // (in) 圧縮時計算レイヤー数
                pCoef_a_layer           // (in) 係数データ
                                        //         [numCalculatedLayer][numStep];
            );

    delete [] pCoef_a_layer;

  }
  
  delete [] pCoef_a_in_region;
  delete [] nstep_coef_ranks;

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcCompressPod::WritePodCoefFile()  End\n");
#endif

  return ret;
}
