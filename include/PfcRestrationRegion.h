#ifndef _PFC_RESTRATION_REGION_H_
#define _PFC_RESTRATION_REGION_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcRestrationRegion.h 分割領域クラス
 * @brief  CPfcRestrationRegion Class Header
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
//#include <set>
//#include <map>
#include <vector>
#include <string>

#include "PfcFileInfo.h"
#include "PfcCompressInfo.h"
#include "PfcDomain.h"
#include "PfcRegion.h"
#include "PfcRestrationRegionPod.h"


/** PFC restraion 領域 class */
class CPfcRestrationRegion {

public:

protected :

  // index.pfc情報
  CPfcFileInfo*     m_pFileInfo;        // File情報（属性etc.）
  CPfcCompressInfo* m_pCompressInfo;    // 圧縮情報
  vector<int>*      m_pvTimeStepList;   // タイムステップリスト(vecterのpointer)

  // proc.pfc関連
  CPfcDomain*       m_pDomain;      //  ドメイン情報
  CPfcRegion*       m_pRegion;      //  分割領域情報（１領域）
  
  // 圧縮のタイプ
  PFC::E_PFC_COMPRESS_FORMAT m_compressForm;    // 圧縮形式 enum値
  
  CPfcRestrationRegionPod*  pPod;   // POD領域クラス（１領域）
  
  bool              m_bLoadCompressData;    // 圧縮データのメモリロードフラグ


public:
  /** コンストラクタ */
  CPfcRestrationRegion();
  
  /**　デストラクタ */
  ~CPfcRestrationRegion();

  /**
   * @brief 展開クラス 初期化
   * @param [in] pFileInfo      File情報（属性etc.）
   * @param [in] pCompressInfo  圧縮情報
   * @param [in] pvTimeStepList タイムステップリスト
   * @param [in] pDomain        ドメイン情報
   * @param [in] pRegion        分割領域情報（１領域）
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  Init(
           CPfcFileInfo*     pFileInfo,       // [in] File情報（属性etc.）
           CPfcCompressInfo* pCompressInfo,   // [in] 圧縮情報
           vector<int>*      pvTimeStepList,  // [in] タイムステップリスト(vecterのpointer)
           CPfcDomain*       pDomain,         // [in] ドメイン情報
           CPfcRegion*       pRegion          // [in] 分割領域情報（１領域）
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
   * @brief データ読み込み（範囲指定）
   * @details  LoadCompressDataOnMem()が実行されている場合、メモリから展開する
   *           対象計算領域と自身が管理している領域が一致していない場合、AND領域のみが設定される
   * @param [out] v             展開後の出力領域 
   *                                領域サイズ：head-tail間のボクセルサイズ*成分数
   *                                設定領域：out_offset_stからout_size分
   * @param [in]  step          ステップ番号
   * @param [in]  head          計算領域の開始位置
   *                                (自身の領域のサイズではない）
   * @param [in]  tail          計算領域の終了位置
   *                                (自身の領域のサイズではない）
   * @param [out] out_offset_st 計算領域の終了位置
   * @param [out] out_size      計算領域の終了位置
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ReadDataInRange (
              double*  v,
              const int step, 
              const int head[3], 
              const int tail[3],
              int out_offset_st[3],
              int out_size  [3]
           );


  /**
   * @brief データ読み込み（位置指定）
   * @details  LoadCompressDataOnMem()が実行されている場合、メモリから展開する
   * @param [out] v         展開後の出力領域 （１要素） 
   * @param [in]  step      ステップ番号
   * @param [in]  ix        x position ( >= 1 ) headの開始は1からのため、それに合わせる
   * @param [in]  iy        y position ( >= 1 ) headの開始は1からのため、それに合わせる
   * @param [in]  iz        z position ( >= 1 ) headの開始は1からのため、それに合わせる
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  ReadData (
              double*  v,
              const int step, 
              const int ix,
              const int iy, 
              const int iz
           );

};
#endif // _PFC_RESTRATION_REGION_H_
