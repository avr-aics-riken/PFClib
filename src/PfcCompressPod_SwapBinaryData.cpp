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


// #################################################################
// バイナリスワップ
PFC::E_PFC_ERRORCODE
CPfcCompressPod::SwapBinaryData(
           double*    pPod_base_r,    // (in)
           int&       base_size,      // (in/out)
           double*    &pFlowData      // (in/out)
                                      //      ポインタの参照渡しを使っていることに注意
                                      //      アドレス自体が変更される
       )
{
  int size_H1 = (int)(base_size/2);     // Half data size
  int size_H2 = base_size - size_H1;
  
  int irank_sendrecv;   // 送受信ランク
  int ist_send;         // pPod_base_r 送信開始位置
  int nsize_send;       // 送信サイズ
  int ist_recv;         // pFlowData 受信開始位置
  int nsize_recv;       // 送受信サイズ


  int c_b = 1;
  for(int c_b_i = 0; c_b_i < m_layerNo + 1; c_b_i++) {
        c_b = c_b * 2;
  }
  int c_interval = 1;
  for(int c_b_i = 0; c_b_i < m_layerNo; c_b_i++) {
    c_interval = c_interval * 2;
  }
  
  //----- lower rank -------------------
  if( m_myIDinRegion % c_b < c_interval)
  {
    pFlowData = new double[size_H1*2];
    
    for(int i=0; i<size_H1; i++ ) {
      pFlowData[i] = pPod_base_r[i];
    }
    
    irank_sendrecv = m_myRankID + c_interval;
    
    ist_send   = size_H1;
    nsize_send = size_H2;
    
    ist_recv   = size_H1;
    nsize_recv = size_H1;
    

    base_size = size_H1; // (out)
  }
  // ----- upper rank -------------------
  else
  {
    pFlowData = new double[size_H2*2];

    for(int i=0; i<size_H2; i++ ) {
      pFlowData[size_H2+i] = pPod_base_r[size_H1+i];
    }
    
    irank_sendrecv = m_myRankID - c_interval;
    
    ist_send   = 0;
    nsize_send = size_H1;
    
    ist_recv   = 0;
    nsize_recv = size_H2;

    base_size = size_H2; // (out)
  }

  // 非同期通信：スワップデータ設定
  MPI_Request req[2];
  MPI_Status  status[2];
  int msgTag = 2000;
  
  PFC_PRINT(" Test SwapBinaryData() Communication start\n" );
      
  // send
  MPI_Isend( &pPod_base_r[ist_send],
                    nsize_send,  MPI_DOUBLE,
                    irank_sendrecv, msgTag,
                    m_comm, &req[0] );

    
  // receive
  MPI_Irecv( &pFlowData[ist_recv], 
                    nsize_recv,  MPI_DOUBLE,
                    irank_sendrecv, msgTag,
                    m_comm, &req[1] );
    
  // mpiwaitall
  MPI_Waitall( 2, req, status );

  PFC_PRINT(" Test SwapBinaryData() Communication  end\n" );

  return PFC::E_PFC_SUCCESS;
}
