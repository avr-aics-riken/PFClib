#ifndef _PFC_DIVISION_H_
#define _PFC_DIVISION_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcDivision.h
 * @brief  PfcDivision Class Header
 * @author aics
 */

#include "PfcDefine.h"
#include "PfcTextParser.h"
#include "PfcRegion.h"

/** proc.pfc ファイルのDivision */
class CPfcDivision {

public:

  int               m_numRegion;
  vector<CPfcRegion> m_regionList;

  int **m_MinMax;           // MinMaxチェック用アクセスインデックス
                            //    [regionID][6]
                            //            [][0]:head[0]  [][1]:head[1]  [][2]:head[2]
                            //            [][3]:tail[0]  [][4]:tail[1]  [][5] tail[2]
   int *m_MinMax_data;      // MinMaxチェック用データ（サイズ：6*領域数）

  /** コンストラクタ */
  CPfcDivision();

  /** デストラクタ */
  ~CPfcDivision();

  /**
   * @brief read Rank(proc.dfi)
   * @param [in]   tpCntl  PfcTextParserクラス 
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Read(CPfcTextParser tpCntl); 

  /**
   * @brief 読込み領域リストの取得
   * @param [in]  head          HeadIndex
   * @param [in]  tail          TailIndex
   * @param [out] regionIdList  読込み領域IDリスト
   * @return error code
   */
  PFC::E_PFC_ERRORCODE 
  CheckReadRegion(
                const int head[3],
                const int tail[3],
                vector<int> &readRegionIdList
               );


  /**
   * @brief 読込み領域IDの取得
   * @param [in]  ix         X インデックス
   * @param [in]  iy         Y インデックス
   * @param [in]  iz         Z インデックス
   * @param [out] regionID   読込み領域ID
   * @return error code
   */
  PFC::E_PFC_ERRORCODE 
  CheckReadRegion(
                const int ix,
                const int iy,
                const int iz,
                int&      regionID
              ); 

  /**
   * @brief PFCファイル:Divisionを出力する
   * @param [in] fp          ファイルポインタ
   * @param [in] tab         インデント
   * @return true:出力成功 false:出力失敗
   */
  PFC::E_PFC_ERRORCODE
  Write(FILE* fp, 
        const unsigned tab);

protected:
 /**
   * @brief MinMax領域設定
   * @return error code
   */
  void
  CreateMinMax( void );

};

#endif // _PFC_DIVISION_H_
