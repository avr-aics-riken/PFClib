#ifndef _PFC_TIMESLICE_H_
#define _PFC_TIMESLICE_H_


/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcTimeSlice.h
 * @brief  CPfcTimeSlice Class Header
 * @author aics
 */

#include "cio_Define.h"
#include "cio_TextParser.h"
#include "cio_TimeSlice.h"

/** index.pfc ファイルの TimeSlice */
//   CIOlib のcio_TimeSliceを派生させて使う
class CPfcTimeSlice : public cio_TimeSlice {

public:

  /** コンストラクタ **/
  CPfcTimeSlice();

 /** コンストラクタ **/
  CPfcTimeSlice( const cio_TimeSlice* cioTimeSlice );

  /** デストラクタ **/
  ~CPfcTimeSlice();

public:

  /**
   * @brief タイムスライス情報を指定された開始ステップ、終了ステップに絞る
   * @param [in]   startStep    開始ステップ
   * @param [in]   endStep      終了ステップ
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Select( 
          int startStep,
          int endStep
        ); 

};

#endif // _PFC_TIMESLICE_H_
