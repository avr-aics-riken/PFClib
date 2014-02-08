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
#include "PfcMatrix.h"


// #################################################################
// 基底計算
PFC::E_PFC_ERRORCODE
CPfcCompressPod::CalcPodBase(
           double*    pFlowData,  // (in)
           const int  unit,       // (in)  領域内のサイズ
           const int  size,       // (in)  タイムステップ方向のサイズ
           double*    pPod_base_r // (out)
         )
{
  double* eigvec;
  double* eigval;
  double* XT;
  double* XTX;

  eigvec = new double[size*size];
  eigval = new double[size];
  XT  = new double[unit*size]; //[unit][size]
  XTX = new double[size*size];

  PFC_matrix_Trans_d(XT, pFlowData, unit, size); // 次元入れ替え

  PFC_dgemm(size, unit, 1.0, XT, pFlowData, 0.0, XTX);

  PFC_dgeev(XTX, size, eigval, eigvec);


  //-------------calculate pod_r-----------------------------------------------//
#ifdef DEBUG_PFC
  double sum = 0.0;
  for(int i = 0; i < size; i++){
    //cout<<"rank is : "<<myid<<" : eigenval "<<i<<" =  "<<eigval[i]<<endl;
    DEBUG_PRINT("  eigenval[%d]=%e\n",i,eigval[i]);
    sum = sum + fabs(eigval[i]);
  }
#endif
  //---------------------------------------------------------------------------//
  
  //-------------calculate the pod base-------------//
  PFC_dgemv(unit, size, 1.0, pFlowData, &eigvec[0], 0.0, pPod_base_r);

  double dwk = 1.0/sqrt(eigval[0]);
  for(int i=0; i<unit; i++) {
    pPod_base_r[i] *= dwk;
  }

  //free memory
  delete []XT;
  delete []XTX;
  delete []eigvec;
  delete []eigval;
  
  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// 係数計算
void
CPfcCompressPod::CalcPodCoef(
                double*   pPod_base_r,    // (in)
                double*   pFlowData,      // (in)
                const int unit,           // (in)  領域内のサイズ
                const int size,           // (in)  タイムステップ方向のサイズ
                double*   pCoef_a         // (out)
            )
{
  double temp;
  
  for(int i = 0; i < size; i++){
    temp = PFC_ddot(unit, &pFlowData[unit*i], pPod_base_r);
    
    pCoef_a[i] = temp;
  }

}

