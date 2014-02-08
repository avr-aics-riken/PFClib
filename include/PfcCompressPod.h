#ifndef _PFC_COMPRESS_POD_H_
#define _PFC_COMPRESS_POD_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompressPod.h
 * @brief  CPfcCompressPod Class Header
 * @author aics
 */

#include "PfcDefine.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <typeinfo>
#include <string>
#include <vector>


/** PFC Compress  main class */
class CPfcCompressPod {

public:

protected :

  MPI_Comm      m_comm;                 // MPI コミュニケータ
  std::string   m_outDirPath;           // フィールドデータの出力ディレクトリパス
  std::string   m_prefix;               // ベースファイル名  （属性名）
  double        m_compressError;        // 誤差率(%)  ユーザ指定誤差
  int           m_numRegion;            // 全体の分割領域数
  int           m_regionSize;           // 圧縮領域のサイズ
  int           m_numStep;              // タイムステップ数
  int           m_optFlags;             // オプション フラグ  （マージした値）
                                        //  == 0 オプションなし
  double*       m_pFlowData;            // 圧縮対象データ領域
                                        //   サイズ：regionSize*myNumStep
                                        //   圧縮時にサイズが変わるので注意
  int           m_myNumStep;            // 自身が担当するステップ数(Layer0)

  
  
  // 属性情報 

  // 全体としての情報
  int           m_numRank;              // 全体のランク数( m_comm内 )
                                        //    = m_numRegion*m_numParallel

  int           m_numParallel;          // 時間軸方向の並列数（分割領域内のランク数）
                                        //   m_numTimeStepから求まる値
                                        
  int           m_maxLayer;             // POD圧縮時の最大レイヤー数
  int           m_calculatedLayer;      // 計算レイヤー数 (最終m_layerNo-1)


  // 分割領域毎の情報
  int           m_regionID;             // 分割領域のID : 0～(m_numTotalRegion-1)
  int           m_regionMasterRankID;   // 分割領域内のマスターランクID
                                        //   分割領域内のランクIDの先頭
                                        //   分割領域内の情報はマスターランクIDに集めて出力する
  int           m_regionMaxStep;        // 分割領域内のランクの最大担当ステップ数

  
  // ランク毎の情報
  int           m_myRankID;             // 自身のランク番号 : 0～(m_numRank-1)
  int           m_myIDinRegion;         // 自身の領域内のID : 0～(m_numParallel-1)
  int           m_myStartStepPos;       // 自身が担当する開始ステップ位置
  
  // 実行時のカレント情報
  int           m_layerNo;              // レイヤーNO  ( 0～ )
  int           m_curPodBaseSize;       // カレントの基底データサイズ
  int           m_curNumSize;           // カレントの時間軸方向のサイズ
  double        m_curEvaluateError;     // カレントのレイヤー積算誤差率(%)
                                        //     分割領域内の積算

  double*       m_pPod_base_r;          // 基底データ
  double*       m_pCoef_a;              // 係数データ


public:
  /** コンストラクタ */
  CPfcCompressPod();
  
  /**　デストラクタ */
  ~CPfcCompressPod();

  /**
   * @brief 圧縮クラス 初期化
   * @param [in]  comm            MPIコミュニケータ
   * @param [in]  outDirPath      フィールドデータ出力ディレクトリパス
   * @param [in]  compressError   誤差率(%)  ユーザ指定誤差
   * @param [in]  numRegion       領域の分割数
   * @param [in]  regionSize      分割領域サイズ（成分数含む）
   * @param [in]  numStep         タイムステップ数
   * @param [in]  optFlags        オプション
   * @param [in]  pFlowData       圧縮するデータ領域 （サイズ：regionSize*myNumStep）
   * @param [in]  myNumStep       自身が担当するステップ数
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  Init(
           const MPI_Comm     comm, 
           const std::string& outDirPath,       // [in] フィールドデータの出力ディレクトリパス
           const std::string& prefix,           // [in] ベースファイル名  （属性名）
           const double       compressError,    // [in] 誤差率(%)  ユーザ指定誤差
           const int          numRegion,        // [in] 分割領域数
           const int          regionSize,       // [in] 分割領域サイズ（成分数含む）
           const int          numStep,          // [in] タイムステップ数
           const int          optFlags,         // [in] オプション
           double*            pFlowData,        // [in] 圧縮するデータ領域
                                                //        サイズ：regionSize*myNumStep
                                                //          ランク毎にサイズが違うので注意
           const int          myNumStep         // [in] 自身が担当するステップ数
       );


  /**
   * @brief 圧縮＆圧縮ファイル出力
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  WriteData( void );

  /**
   * @brief POD 計算レイヤー数取得
   * @return 計算レイヤー数
   */
  int
  GetCalculatedLayer( void );

protected :

  /**
   * @brief PODファイル出力  （基底ファイル,係数ファイル）
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  Output( void );

 /**
   * @brief 基底計算
   * @param [in]  pFlowData   [size][unit]
   * @param [in]  unit        領域内のサイズ
   * @param [in]  size        タイムステップ方向のサイズ
   * @param [out] pPod_base_r [unit]
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  CalcPodBase(
           double*    pFlowData,
           const int  unit,
           const int  size,
           double*    pPod_base_r
         );

 /**
   * @brief 係数計算
   * @param [in]  pPod_base_r  [unit]
   * @param [in]  pFlowData    [size][unit]
   * @param [in]  unit        領域内のサイズ
   * @param [in]  size        タイムステップ方向のサイズ
   * @param [out]  pCoef_a     [size]
   * @return 終了コード
   */
  void
  CalcPodCoef(
                double*   pPod_base_r,    // (in)
                double*   pFlowData,      // (in)
                const int unit,           // (in)
                const int size,           // (in)
                double*   pCoef_a         // (out)
            );

 /**
   * @brief 圧縮終了判定
   * @param [in]    layerNo          レイヤーＮｏ
   * @param [in]    base_size        基底データサイズ
   * @param [in]    num_size         タイムステップ方向のサイズ
   * @param [in]    nalloc_size      タイムステップ方向のアロケーションサイズ
   * @param [in]    userInputError   ユーザ入力誤差率
   * @param [inout] evaluationError  積算誤差率
   * @param [in]    pFlowData        基底データ（バイナリスワップ後）
   * @param [in]    pPod_base_r      基底データ
   * @param [in]    pCoef_a          係数データ
   * @param [out]   finish_flg       終了フラグ
   *                                   =0 not finished  =1 finish
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  CheckFinish(
                const int       layerNo,            // (in)
                const int       base_size,          // (in)
                const int       num_size,           // (in)
                const int       nalloc_size,            // (in)
                const double    userInputError,     // (in)
                double&         evaluationError,    // (in/out) 
                const double*   pFlowData,          // (in)
                const double*   pPod_base_r,        // (in)
                const double*   pCoef_a,            // (in)
                int&            finish_flg          // (out)
            );

 /**
   * @brief バイナリスワップ
   * @param [in]     pPod_base_r  基底データ
   * @param [inout]  base_size    基底データサイズ
   * @param [inout]  pFlowData    基底データ（バイナリスワップ後）
   *                                ポインタのアドレスも更新される
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  SwapBinaryData(
                double*   pPod_base_r,    // (in)
                int&      base_size,      // (in/out) 
                double*   &pFlowData      // (in/out)
                                          //      ポインタの参照渡しを使っていることに注意
                                          //      アドレス自体が変更される
            );

  // POD 基底ファイル 出力
  PFC::E_PFC_ERRORCODE
  WritePodBaseFile(
                    double*   pPod_base_r     // (in) 基底データ
                                              //          サイズ：m_curPodBaseSize
                );

  // POD 基底ファイル 出力（デバッグ出力）
  //   途中のレイヤーの出力：各ランクで別々に出力する
  PFC::E_PFC_ERRORCODE
  WritePodBaseFile_Debug(
                    double*   pPod_base_r     // (in) 基底データ
                                              //          サイズ：m_curPodBaseSize
                );


  // POD 係数ファイル 出力
  PFC::E_PFC_ERRORCODE
  WritePodCoefFile(
                    double*   pCoef_a         // (in) 係数データ
                                              //          サイズ：[m_regionMaxStep][m_calculatedLayer]
                );

};
#endif // _PFC_COMPRESS_POD_H_
