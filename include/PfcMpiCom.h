#ifndef _PFC_MPI_COM_H_
#define _PFC_MPI_COM_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcMpiCom.h
 * @brief   CPfcMpiCom MPI communication Class Header
 * @author aics
 */

#include "PfcDefine.h"
#include <stdlib.h>
#include <string>


/** PFC MPI communication class */
class CPfcMpiCom {

public:

  /**
   * @brief データギャザー Double
   * @param [in]  comm          MPI コミュニケータ
   * @param [in]  masterRankID  マスターランクID（データ収集ランク）
   *                                収集対象ランクの最小ランクNo
   * @param [in]  numRank       収集対象ランク数
   *                                (masterRankID)～(masterRankID+numRank-1)
   * @param [in]  myRankID      自身のランクID
   * @param [in]  nSize         データサイズ（全ランク同一サイズ）
   * @param [in]  localData     ローカルデータ
   * @param [out] gatherData    収集データ（マスターランクのみ設定）
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GatherDataDouble(
                MPI_Comm  comm,               // (in)  MPI コミュニケータ
                const int masterRankID,       // (in)  マスターランクID（データ収集ランク）
                                              //           収集対象ランクの最小ランクNo
                const int numRank,            // (in)  収集対象ランク数
                                              //          (masterRankID)～(masterRankID+numRank-1)
                const int myRankID,           // (in)  自身のランクID
                const int nSize,              // (in)  データサイズ（ローカルデータサイズ）
                                              //          全ランク同一サイズ
                double*   localData,          // (in)  ローカルデータ
                double*   gatherData          // (out) 収集データ
       );


  /**
   * @brief データギャザー Int
   * @param [in]  comm          MPI コミュニケータ
   * @param [in]  masterRankID  マスターランクID（データ収集ランク）
   *                                収集対象ランクの最小ランクNo
   * @param [in]  numRank       収集対象ランク数
   *                                (masterRankID)～(masterRankID+numRank-1)
   * @param [in]  myRankID      自身のランクID
   * @param [in]  nSize         データサイズ（全ランク同一サイズ）
   * @param [in]  localData     ローカルデータ
   * @param [out] gatherData    収集データ（マスターランクのみ設定）
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GatherDataInt(
                MPI_Comm  comm,               // (in)  MPI コミュニケータ
                const int masterRankID,       // (in)  マスターランクID（データ収集ランク）
                                              //           収集対象ランクの最小ランクNo
                const int numRank,            // (in)  収集対象ランク数
                                              //          (masterRankID)～(masterRankID+numRank-1)
                const int myRankID,           // (in)  自身のランクID
                const int nSize,              // (in)  データサイズ（ローカルデータサイズ）
                                              //          全ランク同一サイズ
                int*      localData,          // (in)  ローカルデータ
                int*      gatherData          // (out) 収集データ
       );
       

  /**
   * @brief データギャザー Double 各ランク違うサイズ
   * @param [in]  comm          MPI コミュニケータ
   * @param [in]  masterRankID  マスターランクID（データ収集ランク）
   *                                収集対象ランクの最小ランクNo
   * @param [in]  numRank       収集対象ランク数
   *                                (masterRankID)～(masterRankID+numRank-1)
   * @param [in]  myRankID      自身のランクID
   * @param [in]  nSize         データサイズ（全ランク同一サイズ）
   * @param [in]  localData     ローカルデータ
   * @param [out] gatherData    収集データ（マスターランクのみ設定）
   * @param [out] nSizeRanks    各ランクのサイズ（マスターランクのみ設定）
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GatherV_DataDouble(
                MPI_Comm  comm,               // (in)  MPI コミュニケータ
                const int masterRankID,       // (in)  マスターランクID（データ収集ランク）
                                              //           収集対象ランクの最小ランクNo
                const int numRank,            // (in)  収集対象ランク数
                                              //          (masterRankID)～(masterRankID+numRank-1)
                const int myRankID,           // (in)  自身のランクID
                const int nSize,              // (in)  データサイズ（ローカルデータサイズ）
                                              //          各ランクで違うサイズ
                double*   localData,          // (in)  ローカルデータ
                double*   gatherData,         // (out) 収集データ
                int*      nSizeRanks          // (out) 各ランクのサイズ
       );


  /**
   * @brief MAX値取得 Int
   * @param [in]  comm          MPI コミュニケータ
   * @param [in]  masterRankID  マスターランクID（データ収集ランク）
   *                                収集対象ランクの最小ランクNo
   * @param [in]  numRank       収集対象ランク数
   *                                (masterRankID)～(masterRankID+numRank-1)
   * @param [in]  myRankID      自身のランクID
   * @param [in]  val           自ランクの値
   * @param [out] maxVal        max値（マスターランクのみ設定）
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GetMaxInt(
                MPI_Comm    comm,               // (in)  MPI コミュニケータ
                const int   masterRankID,       // (in)  マスターランクID（データ収集ランク）
                                                //           収集対象ランクの最小ランクNo
                const int   numRank,            // (in)  収集対象ランク数
                                                //          (masterRankID)～(masterRankID+numRank-1)
                const int   myRankID,           // (in)  自身のランクID
                int         val,                // (in)  自ランクの値
                int&        maxVal              // (out) max値（マスターランクのみ設定）
       );
                        
};

#endif // _PFC_MPI_COM_H_
