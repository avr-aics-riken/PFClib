/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcTimeSlice.cpp
 * @brief  CPfcTimeSlice Class
 * @author aics
 */

#include "PfcDefine.h"
#include "PfcTimeSlice.h"


// #################################################################
// コンストラクタ
CPfcTimeSlice::CPfcTimeSlice()
{

}

// コンストラクタ
CPfcTimeSlice::CPfcTimeSlice( const cio_TimeSlice* cioTimeSlice )
{
  SliceList.clear();
  SliceList = cioTimeSlice->SliceList;

  // Average出力フラグの設定
  for(int i=0; i<SliceList.size(); i++ ) {
    if( SliceList[i].AveragedStep >= 0 ) {
      SliceList[i].avr_mode = false;  // Average出力あり
    } else {
      SliceList[i].avr_mode = true;   // Average出力なし
    }
  }
}

// デストラクタ
CPfcTimeSlice::~CPfcTimeSlice()
{

}


// #################################################################
// FilePath 読込み関数
PFC::E_PFC_ERRORCODE
CPfcTimeSlice::Select( 
                     int startStep, // [in]   startStep 開始ステップ
                     int endStep    // [in]   endStep   終了ステップ
        )
{
  // 最初と最後のステップを確認して
  // 絞る必要がないかどうか確認する
  if( SliceList[0].step == startStep && 
      SliceList[SliceList.size()-1].step == endStep    ) {
    // nothing
    return PFC::E_PFC_SUCCESS;
  }
  
  vector<cio_Slice> tmpList = SliceList;
  SliceList.clear();
  
  int ist  = -1;
  int iend = -1;

  for(int i=0; i<tmpList.size(); i++ ) {
    if( tmpList[i].step == startStep )  ist  = i;
    if( ist >=0 && iend == -1 ) {
      SliceList.push_back(  tmpList[i] );
    }
    if( tmpList[i].step == endStep )    iend = i;
  }
  
  if( ist==-1 || iend == -1 ) {
     PFC_PRINT("\t CPfcTimeSlice::Select()  startStep,endStep not matched %d %d\n",startStep,endStep);
    return PFC::E_PFC_ERROR;
  }

  return PFC::E_PFC_SUCCESS;
}

