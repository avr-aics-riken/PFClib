#ifndef _PFC_FUNCTION_H_
#define _PFC_FUNCTION_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcPodFunction.h
 * @brief   PfcPodFunction Class Header  POD圧縮用の関数
 * @author aics
 */

#include "PfcDefine.h"
#include <stdlib.h>
#include <string>

// 整数のべき乗
inline int int_pow(int x, int index)
{
  int result = 1;
  for(int i = 0; i < index; i++)
    result = result * x;
  return result;
}


/** PFC POD function class */
class CPfcFunction {

public:

  /**
   * @brief 領域範囲(Head/Tail)計算
   * @detail  ex) 95 を10で分割する場合
   *                ->  10 10 10 10 10  9  9  9  9  9
   * @param [in]  voxel     ボクセル数
   * @param [in]  div       計算空間の領域分割数
   * @param [in]  regionID  領域ID（１次元）:  0～(div[0]*div[1]*div[2]-1)
   * @param [out] head      計算領域の開始位置 (1起点）
   * @param [out] tail      計算領域の終了位置 (1起点）
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  CalcHeadTail(
                  const int voxel[3], 
                  const int div[3], 
                  const int regionID, 
                  int       head[3],
                  int       tail[3]
                );


  /**
   * @brief 領域範囲(Head/Tail)計算（単純ブロック分割）
   * @detail  ex) 95 を10で分割する場合
   *                ->  10 10 10 10 10 10 10 10 10  5
   * @param [in]  voxel     ボクセル数
   * @param [in]  div       計算空間の領域分割数
   * @param [in]  regionID  領域ID（１次元）:  0～(div[0]*div[1]*div[2]-1)
   * @param [out] head      計算領域の開始位置 (1起点）
   * @param [out] tail      計算領域の終了位置 (1起点）
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  CalcHeadTail_block(
                  const int voxel[3], 
                  const int div[3], 
                  const int regionID, 
                  int       head[3],
                  int       tail[3]
                );


  /**
   * @brief データコピー（範囲指定）
   * @param [out] to             コピー先配列のアドレス
   * @param [in]  nsize_to       コピー先配列のサイズ
   * @param [in]  offset_to      コピー先の配列先頭からのオフセット(0～)
   * @param [in]  from           コピー元配列のアドレス
   * @param [in]  nsize_from     コピー元配列のサイズ
   * @param [in]  offset_from    コピー元の配列先頭からのオフセット(0～)
   * @param [in]  nsize_copy     コピーサイズ
   * @param [in]  num_component  成分数
   * @param [in]  array_shape    配列形状    E_PFC_IJKN / E_PFC_NIJK
   * @return 終了コード
   */
  static void
  CopyData(
                  double*   to,
                  const int nsize_to [3],
                  const int offset_to[3],
                  const double*   from,
                  const int nsize_from [3],
                  const int offset_from[3],
                  const int nsize_copy [3],
                  const int num_component,
                  PFC::E_PFC_ARRAYSHAPE array_shape
           );


  /**
   * @brief Min/Max(Head/Tail)領域 のＡＮＤ（重なり）取得
   * @param [in]  head1          計算領域１の開始位置
   * @param [in]  tail1          計算領域１の終了位置
   * @param [in]  head2          計算領域２の開始位置
   * @param [in]  tail2          計算領域２の終了位置
   * @param [out] lap_offset1    計算領域１の重なり区間までのオフセット
   * @param [out] lap_offset2    計算領域２の重なり区間までのオフセット
   * @param [out] lap_size       ＡＮＤ（重なり）領域のサイズ
   * @return 終了コード   E_PFC_SUCCESS 重なりあり
   *                      E_PFC_ERROR   重なりなし
   */
  static PFC::E_PFC_ERRORCODE
  AndMinMax(
                  const int head1[3],
                  const int tail1[3],
                  const int head2[3],
                  const int tail2[3],
                  int       lap_offset1[3],
                  int       lap_offset2[3],
                  int       lap_size[3]
           );


  /**
   * @brief Min/Max(Head/Tail)領域 の重なりチェック
   * @param [in]  head1          計算領域１の開始位置
   * @param [in]  tail1          計算領域１の終了位置
   * @param [in]  head2          計算領域２の開始位置
   * @param [in]  tail2          計算領域２の終了位置
   * @return 終了コード   E_PFC_SUCCESS 重なりあり
   *                      E_PFC_ERROR   重なりなし
   */
  static PFC::E_PFC_ERRORCODE
  CheckLapMinMax(
                  const int head1[3],
                  const int tail1[3],
                  const int head2[3],
                  const int tail2[3]
           );


  /**
   * @brief 点のMin/Max(Head/Tail)領域内チェック
   * @param [in]  head          計算領域の開始位置
   * @param [in]  tail          計算領域の終了位置
   * @param [in]  ix            点のX座標
   * @param [in]  iy            点のY座標
   * @param [in]  iz            点のZ座標
   * @return 終了コード   E_PFC_SUCCESS 領域内
   *                      E_PFC_ERROR   領域外
   */
  static PFC::E_PFC_ERRORCODE
  CheckPntInMinMax(
                  const int head[3],
                  const int tail[3],
                  const int ix,
                  const int iy,
                  const int iz
           );


  /**
   * @brief ２次元固定長のアロケーション（整数用）
   * @param [in]  size1   １次元目サイズ
   * @param [in]  size2   ２次元目サイズ
   * @param [out] data    アロケーションされた領域（１次元連続領域）
   *                          サイズ:size2*size1
   *                          ポインタの参照型であることに注意
   *                          使用後, delete [] data
   * @param [out] index   dataへのアクセス用インデックス
   *                          [size1][size2]
   *                          ダブルポインタの参照型であることに注意
   *                          使用後, delete [] index
   * @return なし
   */
  static void
  Alloc2D(
                  const int size1,
                  const int size2,
                  int*  &data,
                  int** &index
         );


  /**
   * @brief ２次元固定長のアロケーション（倍精度用）
   * @param [in]  size1  １次元目サイズ
   * @param [in]  size2  ２次元目サイズ
   * @param [out] data   アロケーションされた領域（１次元連続領域）
   *                         サイズ:size2*size1
   *                         ポインタの参照型であることに注意
   *                         使用後, delete [] data
   * @param [out] index  dataへのアクセス用インデックス
   *                         [size1][size2]
   *                         ダブルポインタの参照型であることに注意
   *                         使用後, delete [] index
   * @return なし
   */
  static void
  Alloc2D(
                  const int size1,
                  const int size2,
                  double*  &data,
                  double** &index
         );

  /**
   * @brief POD Max Layer数取得
   * @param [in]   numStep   ステップ数
   * @return Max Layer数
   */
  static int
  GetPodMaxLayer(
                  const int numStep 
                );

  /**
   * @brief POD 並列数取得
   * @param [in]   numStep   ステップ数
   * @return  並列数
   */
  static int
  GetPodParallel(
                  const int numStep 
                );


  /**
   * @brief POD 担当するステップ数＆ステップ開始位置取得
   * @param [in]   numStep      ステップ数
   * @param [in]   id           担当ID（並列内のID）
   * @param [out]  numStepInId  担当するステップ数
   * @param [out]  startStepPos 担当する開始ステップ位置 (0～）
   * @param [out]  maxStep      領域内の各ランクが担当する最大ステップ数
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GetPodStepInfo(
                  const int numStep,
                  const int id,
                  int&      numStepInId,
                  int&      startStepPos,
                  int&      maxStep 
                );


  /**
   * @brief POD のregionID（領域ID)取得
   * @param [in]   rankID       ランクID
   * @param [in]   numStep      ステップ数
   * @param [out]  regionID     領域ID     (0起点）
   * @param [out]  idInRegion   領域内のID (0起点）
   * @param [out]  mastarRankID 分割領域内のマスターランクID (0起点）
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GetPodRegionID(
                  const int rankID,
                  const int numStep,
                  int&      regionID,
                  int&      idInRegion,
                  int&      mastarRankID
                );


  /**
   * @brief POD のbase index取得
   * @detail  base index: pod baseファイルを出力したランク番号
   * @param [in]  numStep             ステップ数
   * @param [in]  numParallel         並列数
   * @param [in]  numCalculatedLayer  計算レイヤー数
   * @param [in]  stepID              ステップID （0起点）
   * @return base index
   */
  static int
  GetPodBaseIndex(
                  const int numStep,
                  const int numParallel,
                  const int numCalculatedLayer,
                  const int stepID
                );
                        
protected :


};

#endif // _PFC_FUNCTION_H_
