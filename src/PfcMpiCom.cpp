/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcMpiCom.cpp
 * @brief  CPfcMpiCom MPI communication Class
 * @author aics
 */

#include "PfcMpiCom.h"


// #################################################################
// データギャザー Double
//     mpi_send/mpi_recv版
//     mpi_gather版を後で作成するかも
PFC::E_PFC_ERRORCODE
CPfcMpiCom::GatherDataDouble(
                MPI_Comm  comm,               // (in)  MPI コミュニケータ
                const int masterRankID,       // (in)  マスターランクID（データ収集ランク）
                                                    //           収集対象ランクの最小ランクNo
                const int numRank,            // (in)  収集対象ランク数
                                                    //          (masterRankID)～(masterRankID+numRank-1)
                const int myRankID,           // (in)  自身のランクID
                const int nSize,              // (in)  データサイズ（ローカルデータサイズ）
                                                    //          全ランク同一サイズ
                double*   localData,          // (in)  ローカルデータ
                double*   gatherData          // (out) 収集データ（マスターランクのみ設定）
       )
{
  int msgTag = 3000;
  MPI_Status status;
 
  // マスターランクでない場合、マスターランクに送信
  if( myRankID != masterRankID )
  {
    MPI_Send( localData, nSize, 
              MPI_DOUBLE, masterRankID, 
              msgTag, comm);
  }
  // マスターランクの場合、他ランクより受信
  else if( myRankID == masterRankID )
  {
    // 自ランクの分を設定
    for(int i = 0; i < nSize; i++) {
      gatherData[i] = localData[i];
    }
    // 他ランクより受信
    for(int i = 1; i < numRank; i++) {
      MPI_Recv( &gatherData[nSize*i], nSize, 
                MPI_DOUBLE, (masterRankID+i), 
                msgTag, comm, &status );
    }

  }

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// データギャザー Int
//     mpi_send/mpi_recv版
//     mpi_gather版を後で作成するかも
PFC::E_PFC_ERRORCODE
CPfcMpiCom::GatherDataInt(
                MPI_Comm  comm,               // (in)  MPI コミュニケータ
                const int masterRankID,       // (in)  マスターランクID（データ収集ランク）
                                              //           収集対象ランクの最小ランクNo
                const int numRank,            // (in)  収集対象ランク数
                                              //          (masterRankID)～(masterRankID+numRank-1)
                const int myRankID,           // (in)  自身のランクID
                const int nSize,              // (in)  データサイズ（ローカルデータサイズ）
                                              //          全ランク同一サイズ
                int*      localData,          // (in)  ローカルデータ
                int*      gatherData          // (out) 収集データ（マスターランクのみ設定）
       )
{
  int msgTag = 3001;
  MPI_Status status;
 
  // マスターランクでない場合、マスターランクに送信
  if( myRankID != masterRankID )
  {
      MPI_Send( localData, nSize, 
                MPI_INT, masterRankID, 
                msgTag, comm);
  }
  // マスターランクの場合、他ランクより受信
  else if( myRankID == masterRankID )
  {
    // 自ランクの分を設定
    for(int i = 0; i < nSize; i++) {
      gatherData[i] = localData[i];
    }
    // 他ランクより受信
    for(int i = 1; i < numRank; i++) {

      MPI_Recv( &gatherData[nSize*i], nSize, 
                MPI_INT, (masterRankID+i), 
                msgTag, comm, &status );

    }

  }

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// データギャザー Double 各ランク違うサイズ
//     mpi_send/mpi_recv版
//     mpi_gather版を後で作成するかも
PFC::E_PFC_ERRORCODE
CPfcMpiCom::GatherV_DataDouble(
                MPI_Comm  comm,               // (in)  MPI コミュニケータ
                const int masterRankID,       // (in)  マスターランクID（データ収集ランク）
                                                    //           収集対象ランクの最小ランクNo
                const int numRank,            // (in)  収集対象ランク数
                                                    //          (masterRankID)～(masterRankID+numRank-1)
                const int myRankID,           // (in)  自身のランクID
                const int nSize,              // (in)  データサイズ（ローカルデータサイズ）
                                                    //          各ランクで違うサイズ
                double*   localData,          // (in)  ローカルデータ
                double*   gatherData,         // (out) 収集データ（マスターランクのみ設定）
                int*      nSizeRanks          // (out) 各ランクのサイズ（マスターランクのみ設定）
       )
{
  int msgTag = 3002;
  MPI_Status status;

  int nSize_wk = (int)nSize;

  // 各ランクのサイズ取得
  CPfcMpiCom::GatherDataInt(
                comm,               // (in)  MPI コミュニケータ
                masterRankID,       // (in)  マスターランクID（データ収集ランク）
                numRank,            // (in)  収集対象ランク数
                myRankID,           // (in)  自身のランクID
                1,                  // (in)  データサイズ（ローカルデータサイズ）
                &nSize_wk,          // (in)  ローカルデータ
                nSizeRanks          // (out) 各ランクのサイズ
              );
  
  // マスターランクでない場合、マスターランクに送信
  if( myRankID != masterRankID )
  {
    MPI_Send( localData, nSize, 
              MPI_DOUBLE, masterRankID, 
              msgTag, comm);
  }
  // マスターランクの場合、他ランクより受信
  else if( myRankID == masterRankID )
  {
    // 自ランクの分を設定
    for(int i = 0; i < nSize; i++) {
      gatherData[i] = localData[i];
    }

    // 他ランクより受信
    int ip = 0;
    for(int i = 1; i < numRank; i++) {
      ip += nSizeRanks[i-1];
      
      MPI_Recv( &gatherData[ip], nSizeRanks[i], 
                MPI_DOUBLE, (masterRankID+i), 
                msgTag, comm, &status );

    }

  }

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// MAX値取得 Int
//     mpi_send/mpi_recv版
//     mpi_reduce版を後で作成するかも
PFC::E_PFC_ERRORCODE
CPfcMpiCom::GetMaxInt(
                MPI_Comm    comm,               // (in)  MPI コミュニケータ
                const int   masterRankID,       // (in)  マスターランクID（データ収集ランク）
                                                //           収集対象ランクの最小ランクNo
                const int   numRank,            // (in)  収集対象ランク数
                                                //          (masterRankID)～(masterRankID+numRank-1)
                const int   myRankID,           // (in)  自身のランクID
                int         val,                // (in)  自ランクの値
                int&        maxVal              // (out) max値（マスターランクのみ設定）
       )
{
  int msgTag = 3010;
  MPI_Status status;
  
  // マスターランクでない場合、マスターランクに送信
  if( myRankID != masterRankID )
  {
      maxVal = 0.0;

      MPI_Send( &val, 1, 
                MPI_INT, masterRankID, 
                msgTag, comm);
  }
  // マスターランクの場合、他ランクより受信
  else if( myRankID == masterRankID )
  {
    // 自ランクの分を設定
    maxVal = val;

    // 他ランクより受信
    int iwk;
    for(int i = 1; i < numRank; i++) {
      MPI_Recv( &iwk, 1, 
                MPI_INT, (masterRankID+i), 
                msgTag, comm, &status );

      if( iwk > maxVal ) {
        maxVal = iwk;
      }
    }

  }

  return PFC::E_PFC_SUCCESS;
}


