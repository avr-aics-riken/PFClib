#ifndef _PFC_COMPRESS_CIO_POD_H_
#define _PFC_COMPRESS_CIO_POD_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompressCioPod.h
 * @brief  CPfcCompressCioPod Class Header
 * @author aics
 */

#include "PfcDefine.h"
#include "cio_DFI.h"
#include "PfcDivision.h"
#include "PfcCompressPod.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <typeinfo>
#include <string>
#include <vector>


/** PFC CompressCioPod  main class */
class CPfcCompressCioPod {

public:
  // POD用圧縮本体
  CPfcCompressPod* m_pPod;              // POD圧縮用クラス（１次元）

protected :
  // POD用圧縮対象データ（CIOにより読み込んだデータ）
  double*       m_pFlowData;            // 圧縮対象データ領域

  MPI_Comm      m_comm;                 ///< MPI コミュニケータ
  cio_DFI* m_pDfiIN;               // 入力となるDFI情報
  std::string   m_outDirPath;           ///<フィールドデータの出力ディレクトリパス
  double        m_compressError;        // 誤差率(%)  ユーザ指定誤差
  int           m_numRegion;            // 全体の分割領域数
  int           m_regionHead[3];        // 分割計算領域の開始位置
  int           m_regionTail[3];        // 分割計算領域の終了位置
  int           m_numStep;              // 対象とするタイムステップ数
  const vector<int>*  m_pStepList;            // タイムステップリスト（対象となるタイムステップ）
                                        //   0からの連番とは限らないことに注意
  int           m_optFlags;             // オプション フラグ  （マージした値）
                                        //  == 0 オプションなし

  int           m_numRank;              // 全体のランク数( m_comm内 )
  int           m_myRankID;             // 自身のランク番号 : 0～(m_numRank-1)

public:
  /** コンストラクタ */
  CPfcCompressCioPod();
  
  /**　デストラクタ */
  ~CPfcCompressCioPod();

  /**
   * @brief 圧縮クラス 初期化
   * @param [in]  comm            MPIコミュニケータ
   * @param [in]  pDfiIN          CIO DFIクラス
   * @param [in]  outDirPath      フィールドデータ出力ディレクトリパス
   * @param [in]  compressError   誤差率(%)
   * @param [in]  numRegion       分割領域数
   * @param [in]  head            分割計算領域の開始位置
   * @param [in]  tail            分割計算領域の終了位置
   * @param [in]  pStepList       タイムステップリスト
   * @param [in]  optFlags        オプション    0  オプションなし
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  Init(
           const MPI_Comm     comm, 
           cio_DFI*     pDfiIN,
           const std::string& outDirPath,
           const double       compressError,
           const int          numRegion,
           const int          head[3],
           const int          tail[3],
           const vector<int>* pStepList,    // タイムステップリスト（対象となるタイムステップ）
                                            //   0からの連番とは限らないことに注意
           const int          optFlags
       );

  /**
   * @brief 圧縮＆圧縮ファイル出力
   * @param [out] ret     終了コード
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  WriteData( void );

protected :

  /**
   * @brief CIOファイル Read original配列形状(NIJK/IJKN)順
   * @param [in]  pFlowData       読み込みデータ
   * @param [in]  dataType        配列のデータタイプ
   * @param [in]  startStep       読み込み開始ステップ (0からの連番）
   * @param [in]  numStep         時間軸方向のサイズ
   * @param [in]  unit            領域方向のサイズ
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ReadCioFile (
           double*   pFlowData,  // (out) [numStep][unit]
           const PFC::E_PFC_DTYPE dataType, // [in]  配列のデータタイプ
           const int startStep,  // (in)  読み込み開始ステップ (0からの連番）
           const int numStep,    // (in)  時間軸方向のサイズ
           const int unit        // (in)  領域方向のサイズ
       );

  /**
   * @brief CIOファイル Read 配列形状(IJKN)順
   * @detail   POD圧縮内部データをIJKN順とするため
   * @param [in]  pFlowData       読み込みデータ 配列形状(IJKN)順
   * @param [in]  dataType        配列のデータタイプ
   * @param [in]  startStep       読み込み開始ステップ (0からの連番）
   * @param [in]  numStep         時間軸方向のサイズ
   * @param [in]  unit            領域方向のサイズ
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ReadCioFile_IJKN (
           double*   pFlowData,  // (out) [numStep][unit]
           const PFC::E_PFC_DTYPE dataType, // [in]  配列のデータタイプ
           const int startStep,  // (in)  読み込み開始ステップ (0からの連番）
           const int numStep,    // (in)  時間軸方向のサイズ
           const int unit        // (in)  領域方向のサイズ
       );
                                        
                                        

};
#endif // _PFC_COMPRESS_CIO_POD_H_
