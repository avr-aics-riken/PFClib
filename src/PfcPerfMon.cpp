/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcPerfMon.cpp 
 * @brief  PfcPerfMon Function Class
 * @author aics    
 */

#ifdef USE_PMLIB

#include "PfcPerfMon.h"

static PerfMonitor* pPM = NULL;
static int rankID_PM = 0;


// #################################################################
// 初期化（圧縮用）
void
PfcPerfMon::InitCompress( const int n_proc, const int rankID, const int n_thread )
{
  rankID_PM = rankID;

  pPM = new PerfMonitor;

  pPM->setRankInfo( rankID );
  pPM->initialize ( tNUM_c );

  std::string p_mode;
  if( n_proc==1 && (n_thread==0 || n_thread==1) ) {
    p_mode = "Serial";
  } else if( n_proc>1 && (n_thread==0 || n_thread==1) ) { 
    p_mode = "FlatMPI";
  } else if( n_proc==1 && n_thread>1 ) { 
    p_mode = "OpenMP";
  } else if( n_proc>1 && n_thread>1 ) { 
    p_mode = "Hybrid";
  }
  pPM->setParallelMode( p_mode, n_thread, n_proc);


  // プロパティ設定
  //    t10_c,t20_c,t30_c 以外は呼び出し側でタイマー関数をcall

  /* reserve
  pPM->setProperties( t1_c, "T1", PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t2_c, "T2", PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t3_c, "T3", PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t4_c, "T4", PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t5_c, "T5", PerfMonitor::CALC, false );//計算, 非排他
  */

  pPM->setProperties( t10_c   , "T10  CPfcCompress::Init()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t101_c  , "T101 CPfcCompress::Init()-CPfcCioDfiUtil::GetDfiTimeStepList()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t102_c  , "T102 CPfcCompress::Init()-CPfcCioDfiUtil::GetDfiTimeStepList()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t103_c  , "T103 CPfcCompress::Init()-cio_DFI::ReadInit()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t105_c  , "T105 CPfcCompress::Init()-CPfcCompressCioPod::Init()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t110_c  , "T110 CPfcCompressCioPod::Init()-ReadCioFile_IJKN()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t112_c  , "T112 CPfcCompressCioPod::ReadCioFile_IJKN()-ReadCioFile()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t114_c  , "T114 CPfcCompressCioPod::ReadCioFile()-cio_DFI::ReadData()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t120_c  , "T120 CPfcCompressCioPod::Init()-CPfcCompressPod::Init()", 
                                         PerfMonitor::CALC, true ); //計算, 排他

  pPM->setProperties( t20_c   , "T20  CPfcCompress::WriteProcFile()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t201_c  , "T201 CPfcCompress::WriteProcFile()-GatherAndCreateDivisionInfo()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t202_c  , "T202 CPfcCompress::WriteProcFile()-CPfcDomain::Write()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t203_c  , "T203 CPfcCompress::WriteProcFile()-CPfcDivision::Write()", 
                                         PerfMonitor::CALC, false );//計算, 非排他

  pPM->setProperties( t30_c   , "T30  CPfcCompress::WriteData()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t301_c  , "T301 CPfcCompress::WriteData()-CPfcCompressCioPod::WriteData()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t310_c  , "T310 CPfcCompressCioPod::WriteData()-CPfcCompressPod::WriteData()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t312_c  , "T312 CPfcCompressPod::WriteData()-CalcPodBase()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t314_c  , "T314 CPfcCompressPod::WriteData()-CalcPodCoef()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t316_c  , "T316 CPfcCompressPod::WriteData()-CheckFinish()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t318_c  , "T318 CPfcCompressPod::WriteData()-SwapBinaryData()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t330_c  , "T330 CPfcCompressPod::WriteData()-Output()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t332_c  , "T332 CPfcCompressPod::Output()-WritePodBaseFile()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t341_c  , "T341 CPfcCompressPod::WritePodBaseFile()-CPfcMpiCom::GetMaxInt()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t343_c  , "T343 CPfcCompressPod::WritePodBaseFile()-CPfcMpiCom::GatherV_DataDouble()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t345_c  , "T345 CPfcCompressPod::WritePodBaseFile()-CPfcPodFile::WriteBaseFile()", 
                                         PerfMonitor::CALC, false );//計算, 非排他

  pPM->setProperties( t334_c  , "T334 CPfcCompressPod::Output()-WritePodCoefFile()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t302_c  , "T302 CPfcCompress::WriteData()-WriteIndexPfcFile()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
}


// #################################################################
// 初期化（展開用）
void
PfcPerfMon::InitRestore( const int n_proc, const int rankID, const int n_thread )
{
  rankID_PM = rankID;

  pPM = new PerfMonitor;

  pPM->setRankInfo( rankID );
  pPM->initialize ( tNUM_r );

  std::string p_mode;
  if( n_proc==1 && (n_thread==0 || n_thread==1) ) {
    p_mode = "Serial";
  } else if( n_proc>1 && (n_thread==0 || n_thread==1) ) { 
    p_mode = "FlatMPI";
  } else if( n_proc==1 && n_thread>1 ) { 
    p_mode = "OpenMP";
  } else if( n_proc>1 && n_thread>1 ) { 
    p_mode = "Hybrid";
  }
  pPM->setParallelMode( p_mode, n_thread, n_proc);

  // プロパティ設定
  //    t10_r,t20_r,t5**_c 以外は呼び出し側でタイマー関数をcall

  /* reserve
  pPM->setProperties( t1_r, "T1", PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t2_r, "T2", PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t3_r, "T3", PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t4_r, "T4", PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t5_r, "T5", PerfMonitor::CALC, false );//計算, 非排他
  */

  pPM->setProperties( t10_r   , "T10   CPfcRestration::Init()", 
                                         PerfMonitor::CALC, true ); //計算, 排他

  pPM->setProperties( t20_r   , "T20   CPfcRestration::ReadData()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t201_r  , "T201  CPfcRestration::ReadData()-LoadCompressDataOnMem()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t211_r  , "T211  CPfcRestration::LoadCompressDataOnMem()-CPfcDivision::CheckReadRegion()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t213_r  , "T213  CPfcRestration::LoadCompressDataOnMem()-CPfcRestrationRegion::Init()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t215_r  , "T215  CPfcRestration::LoadCompressDataOnMem()-CPfcRestrationRegion::LoadCompressDataOnMem()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t217_r  , "T217  CPfcRestrationRegion::LoadCompressDataOnMem()-CPfcRestrationRegionPod::LoadCompressDataOnMem()", 
                                         PerfMonitor::CALC, false );//計算, 非排他

  pPM->setProperties( t202_r  , "T202  CPfcRestration::ReadData()-CPfcRestrationRegion::ReadDataInRange()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t231_r  , "T231  CPfcRestrationRegion::ReadDataInRange()-CPfcRestrationRegionPod::ReadData()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t251_r  , "T251  CPfcRestrationRegionPod::ReadData()-ReadFieldData()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
  pPM->setProperties( t261_r  , "T261  CPfcRestrationRegionPod::ReadFieldData()-LoadCompressDataOnMem()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t263_r  , "T263  CPfcRestrationRegionPod::ReadFieldData()-ExpandData()", 
                                         PerfMonitor::CALC, false );//計算, 非排他

  pPM->setProperties( t271_r  , "T271  CPfcRestrationRegionPod::-ExpandData()-GetExpandRowInfo()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t273_r  , "T273  CPfcRestrationRegionPod::-ExpandData()-(calc val)", 
                                         PerfMonitor::CALC, true ); //計算, 排他

  pPM->setProperties( t233_r  , "T233  CPfcRestrationRegion::ReadDataInRange()-CPfcFunction::AndMinMax()", 
                                         PerfMonitor::CALC, true ); //計算, 排他
  pPM->setProperties( t235_r  , "T235  CPfcRestrationRegion::ReadDataInRange()-CPfcFunction::CopyData()", 
                                         PerfMonitor::CALC, true ); //計算, 排他

     // 共通
  pPM->setProperties( t501_r  , "T501  CPfcRestration::GetTimeStepList()", 
                                         PerfMonitor::CALC, false );//計算, 非排他
}


// #################################################################
// 終了化（Peformance Monitor結果出力）
void
PfcPerfMon::Term( void )
{
  if( pPM == NULL ) {
     PFC_PRINT("ERROR: Performance Monitor not initialized.\n");
     fflush(stdout);
     //exit(-1);
     return;
  }

  pPM->gather();

  if( rankID_PM == 0 ) {
    std::string hostname("X");
    std::string operatorname("X");
    pPM->print(stdout,hostname,operatorname);

    pPM->printDetail(stdout);
  }

  delete pPM;
  pPM = NULL;
}


// #################################################################
// タイマー開始
void
PfcPerfMon::Start( unsigned key )
{
  /*
  if( pPM == NULL ) {
     PFC_PRINT("ERROR: Performance Monitor not initialized.\n");
     fflush(stdout);
     exit(-1);
  }
  */
  if( pPM == NULL ) return;

  pPM->start( key );
}


// #################################################################
// タイマー終了
void
PfcPerfMon::Stop( unsigned key )
{
  /*
  if( pPM == NULL ) {
     PFC_PRINT("ERROR: Performance Monitor not initialized.\n");
     fflush(stdout);
     exit(-1);
  }
  */
  if( pPM == NULL ) return;

  pPM->stop( key );
}

#endif // USE_PMLIB
