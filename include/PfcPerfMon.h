#ifndef _PFC_PERFMON_H_
#define _PFC_PERFMON_H_

//#ifdef USE_PMLIB

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcPerfMon.h
 * @brief  PfcPferfMon Header
 * @author aics
 */

#include "PfcDefine.h"
#include "PerfMonitor.h"
using namespace pm_lib;

//圧縮用タイマーキー
enum timing_key_compress {
    // t1_c - t5_c : ユーザ使用可能key
    //t1_c,
    //t2_c,
    //t3_c,
    //t4_c,
    //t5_c,

    // プログラム内部のkey : ユーザ使用不可
    t10_c,
    t101_c,
    t102_c,
    t103_c,
    t105_c,
    t110_c,
    t112_c,
    t114_c,
    t120_c,
    t20_c,
    t201_c,
    t202_c,
    t203_c,
    t30_c,
    t301_c,
    t310_c,
    t312_c,
    t314_c,
    t316_c,
    t318_c,
    t330_c,
    t332_c,
    t341_c,
    t343_c,
    t345_c,
    t334_c,
    t302_c,
    // ターミネータ(＝測定区間総数)
    tNUM_c
  };

//展開用タイマーキー
enum timing_key_restore {
    // t1_r - t5_r : ユーザ使用可能key
    //t1_r,
    //t2_r,
    //t3_r,
    //t4_r,
    //t5_r,

    // プログラム内部のkey : ユーザ使用不可
    t10_r,
    t20_r,
    t201_r,
    t211_r,
    t213_r,
    t215_r,
    t217_r,
    t202_r,
    t231_r,
    t251_r,
    t261_r,
    t263_r,
    t271_r,
    t273_r,
    t233_r,
    t235_r,
    t501_r,
    // ターミネータ(＝測定区間総数)
    tNUM_r
  };


/** PFC Performance Monitor class */
class PfcPerfMon {

public:

  // 初期化（圧縮用）
  static void
  InitCompress( const int n_proc, const int rankID, const int n_thread );

  // 初期化（展開用）
  static void
  InitRestore( const int n_proc, const int rankID, const int n_thread );

  // 終了化（Peformance Monitor結果出力）
  static void
  Term( void );

  // タイマー開始
  static void
  Start( unsigned key );

  // タイマー終了
  static void
  Stop( unsigned key );

};

//#endif // USE_PMLIB

#endif // _PFC_PERFMON_H_
