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

#include <math.h>
#include <iostream>
#include "PfcCompressPod.h"
using namespace std;


// #################################################################
// 圧縮終了判定
//     領域内でコミュニケータがあれば簡単であるが、現在ないので send/recvで行う
PFC::E_PFC_ERRORCODE
CPfcCompressPod::CheckFinish(
                const int       layerNo,            // (in)  レイヤーＮｏ
                const int       base_size,          // (in)  基底データのサイズ
                const int       num_size,           // (in)  タイムステップ方向のサイズ
                const int       nalloc_size,        // (in)  タイムステップ方向のアロケーションサイズ
                const double    userInputError,     // (in)  ユーザ入力誤差率 (%)
                double&         evaluationError,    // (in/out) 積算誤差率 (%)
                const double*   pFlowData,          // (in)
                const double*   pPod_base_r,        // (in)  基底データ
                const double*   pCoef_a,            // (out) 係数データ
                int&            finish_flg          // (out)  =0 not finished  =1 finish
       )
{
  const double cutoff_ratio=0.01;  // 0付近は誤差率が大きくなるので無視させるための定数
  
  //-------------------------------------
  //  誤差チェック  デバッグ用として誤差率を出力したいので必ず実行
   //-------------------------------------
  double layer_evaluate_error = 0.0;
  
  for(int i = 0; i < num_size; i++){
    double temp_error = 0.0;

    double fDat_abs_max = 0.0;
    for(int j = 0; j < base_size; j++) {
      double fDat = pFlowData[base_size*i+j];
      if( fabs(fDat) > fDat_abs_max ) {
        fDat_abs_max = fabs(fDat);
      }
    }

    for(int j = 0; j < base_size; j++) {
      double fDat = pFlowData[base_size*i+j];
      // 誤差が大きくなるのを防ぐためのチェック
      //    0付近は誤差率が大きくなるため
      if( fabs(fDat) < (cutoff_ratio*fDat_abs_max) ) {
        continue;
      }  

      double a    = pCoef_a[nalloc_size*layerNo+i];
      
      //if user inputs 5.4, the error should be lower than 5.4%.
      temp_error += fabs( (fDat - a*pPod_base_r[j])/fDat );
    }
    temp_error = (temp_error/base_size)*100.0;    // -> percentage
    
    if(temp_error > layer_evaluate_error) {
      layer_evaluate_error = temp_error;
    }
  }

  double layer_evaluate_error_max = 0.0;

  // ランクID取得
  int myRankID;
  MPI_Comm_rank( m_comm, &myRankID );
  
  // 0ランクに reduce max
  MPI_Reduce( &layer_evaluate_error,
              &layer_evaluate_error_max,
              1, MPI_DOUBLE, MPI_MAX,
              0, m_comm );

  // エラー率積算
  evaluationError = evaluationError + layer_evaluate_error_max;
  
  // デバッグ用として標準出力に誤差率を出力する。
  if( myRankID == 0 ) {
    cout << "**** current_layer_no ="<<m_layerNo<<" Calculated CompressError(%) = "
                <<evaluationError<< "  ****"<< endl;
  }

  // 誤差率判定
  //if evaluate_error > user input error, stop in this layer, else go on the compression in next layer
  if( evaluationError > userInputError ){
    if( myRankID == 0 ) {
      cout << "**** Finished calclulation: UserInputError(%)="<<userInputError<<" Over ****"<< endl;
      cout << "      Number Of CalculatedLayer = "<< (m_layerNo+1) << endl;
    }
    finish_flg = 1; // 終了
  } else {
    finish_flg = 0; // 継続
  }
  
  //-------------------------------------
  // 最大レイヤー数チェック
  //-------------------------------------
  if( layerNo >= (m_maxLayer-1) )  {
    finish_flg = 1; // 終了
    return PFC::E_PFC_SUCCESS;
  }
  
  //-------------------------------------
  // 要素数チェック （２以上ないと分割出来ない）
  //-------------------------------------
  if( base_size <= 1 )  {
    finish_flg = 1; // 終了
    
    PFC_PRINT(" SwapBinaryData() : pod base size can not be devided layerNo=%d\n",layerNo );
  }

  //-------------------------------------
  // 終了フラグ 送信
  //-------------------------------------
  MPI_Bcast(  &finish_flg,
              1, MPI_INT,
              0, m_comm );
  

  return PFC::E_PFC_SUCCESS;
}
