#ifndef _PFC_RESTRATION_REGIONPOD_H_
#define _PFC_RESTRATION_REGIONPOD_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcRestrationRegionPod.h 分割領域クラス（１次元で操作する）
 * @brief  CPfcRestrationRegionPod Class Header
 * @author aics
 */

#include "PfcDefine.h"
#ifdef USE_PMLIB
#include "PfcPerfMon.h"
#endif

#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <typeinfo>
#include <vector>
#include <string>

#include "PfcFileInfo.h"
#include "PfcCompressInfo.h"
#include "PfcDomain.h"
#include "PfcRegion.h"


/** PFC restraion class */
class CPfcRestrationRegionPod {

public:

protected :

  bool              m_bLoadCompressData;    // 圧縮データのメモリロードフラグ
  
  std::string       m_dirPath;          // フィールドデータ出力ディレクトリパス
  std::string       m_prefix;           // 属性名
  int               m_regionID;         // 領域ID
  int               m_numSize;          // 要素数
                                        //      1stepあたりサイズ：m_numSize*m_numComponent
  PFC::E_PFC_ARRAYSHAPE m_arrayShape;   // 配列形状    E_PFC_IJKN / E_PFC_NIJK
  int               m_numComponent;     // 成分数
  int               m_numStep;          // タイムステップ数
  bool              m_bSingle;          // 単一領域フラグ
                                        //    基底および係数ファイルのファイル名の決定に必要


  // 圧縮 基底と係数の共通情報
  int       m_numCalculatedLayer;   // 圧縮時計算レイヤー数
  int       m_numParallel;          // 圧縮時の並列数

  // 圧縮 基底データ情報
  double**  m_pIndexBase;       // 圧縮時の各ランク内の基底データへのポインタ
                                //     m_pIndexBase[m_numParallel]
  double*   m_pBaseData;        // 基底データ
  int*      m_pBaseSizes;       // 圧縮時の各ランク内の要素数
                                //     m_pBaseSizes[m_numParallel]

  // 圧縮 係数データ情報
  int       m_numCoef;          // 係数出力数（Layer1以降）
  double**  m_pIndexCoef;       // 係数データへのポインタ
                                //     [m_numCalculatedLayer]
  double*   m_pCoefData;        // 係数データ
                                //     [numCalculatedLayer][numStep]
                                //     Layer1以降はm_numCoef分のみ有効

public:
  /** コンストラクタ */
  CPfcRestrationRegionPod();
  
  /**　デストラクタ */
  ~CPfcRestrationRegionPod();

  /**
   * @brief 展開クラス 初期化
   * @param [in] dirPath        フィールドデータ出力ディレクトリパス
   * @param [in] prefix         属性名
   * @param [in] regionID       領域ID
   * @param [in] numSize        要素数 (voxelサイズ）
   * @param [in] arrayShape     配列形状    E_PFC_IJKN / E_PFC_NIJK
   * @param [in] numComponent   成分数
   * @param [in] numStep        タイムステップ数
   * @param [in] arrayShape     配列形状    E_PFC_IJKN / E_PFC_NIJK
   * @param [in] bSingle        単一領域フラグ  true / false
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  Init(
           const std::string&      dirPath,        // [in] フィールドデータ出力ディレクトリパス
           const std::string&      prefix,         // [in] 属性名
           const int               regionID,       // [in] 領域ID
           const int               numSize,        // [in] 要素数 (voxelサイズ）
           PFC::E_PFC_ARRAYSHAPE   arrayShape,     // [in] 配列形状    E_PFC_IJKN / E_PFC_NIJK
           int                     numComponent,   // [in] 成分数
           int                     numStep,        // [in] タイムステップ数
           bool                    bSingle         // [in] 単一領域フラグ
      );


  /**
   * @brief 圧縮データのメモリロード（分割領域の全データロード）
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  LoadCompressDataOnMem( void );


  /**
   * @brief ロードした圧縮データ削除
   * @return なし
   */
  void
  DeleteCompressDataOnMem( void );



  /**
   * @brief フィールドデータ読み込み（領域内全情報）配列形状指定
   * @details  指定された配列形状に従ってデータを返す
   * @param [in]  arrayShape   配列形状  (E_PFC_IJKN/E_PFC_NIJK)
   * @param [out] v            展開後の出力領域 
   *                              メモリは確保済みであること
   *                              region内のボクセルサイズ*成分数
   * @param [in]  stepID       ステップID (0起点）
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ReadData (
              PFC::E_PFC_ARRAYSHAPE arrayShape,
              double*  v,
              const int stepID
           );

  /**
   * @brief 圧縮データ展開（領域内全情報,Load済みのデータから展開）
   * @param [out] v         展開後の出力領域 
   *                              メモリは確保済みであること
   *                              ボクセルサイズ*成分数
   * @param [in]  stepID    ステップID (0起点）
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ExpandData (
              double*  v,
              const int stepID
           );


  /**
   * @brief 圧縮データ展開（Load済みのデータから展開）
   * @param [out] v         展開後の出力領域 （１要素）
   * @param [in]  stepID    ステップID (0起点）
   * @param [in]  index     領域先頭からの要素の１次元Index
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ExpandData (
              double*  v,
              const int stepID,
              const int index
           );


  /**
   * @brief 圧縮データ展開（ファイルから展開）
   * @param [out] v         展開後の出力領域 （１要素）
   * @param [in]  stepID    ステップID (0起点）
   * @param [in]  index     領域先頭からの要素の１次元Index
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ExpandDataOnFile (
              double*  v,
              const int stepID,
              const int index
           );



protected:

  /**
   * @brief フィールドデータ読み込み（領域内全情報）
   * @detail  POD圧縮時の配列形状で返す　(IJKN順)
   * @details  LoadCompressDataOnMem()が実行されている場合、メモリから展開する
   * @param [out] v         展開後の出力領域 
   *                              メモリは確保済みであること
   *                              region内のボクセルサイズ*成分数
   * @param [in]  stepID    ステップID (0起点）
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ReadFieldData (
              double*  v,
              const int stepID
           );

  /**
   * @brief 圧縮データ展開用経路情報取得
   * @detail 係数ファイルは読み込み済みであること
   *         基底ファイルのヘッダは読み込み済みであること
   * @param [in]  stepID      ステップID (0起点）
   * @param [in]  row_num   経路の数
   * @param [out] coef      経路毎の係数  size:[row_num]
   * @param [out] base_index  経路毎の基底のインデックス size:[row_num]
   *                              pod baseファイルを出力したランク番号
   * @param [out] row_st      経路毎の開始位置  (0起点）size:[row_num]
   * @param [out] row_size    経路毎のサイズ size:[row_num]
   *                              == base_index毎のサイズ
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  GetExpandRowInfo(
            const int stepID,       // [in]  ステップID
            const int row_num,      // [in]  経路の数
            double*   coef,         // [out] 経路毎の係数  size:[row_num]
            int*      base_index,   // [out] 経路毎の基底のインデックス size:[row_num]
                                    //         pod baseファイルを出力したランク番号
            int*      row_st,       // [out] 経路毎の開始位置  (0起点）size:[row_num]
            int*      row_size      // [out] 経路毎のサイズ size:[row_num]
                                    //         == base_index毎のサイズ
        );

};
#endif // _PFC_RESTRATION_REGIONPOD_H_
