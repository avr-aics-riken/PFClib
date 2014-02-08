#ifndef _PFC_DOMAIN_H_
#define _PFC_DOMAIN_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcDomain.h
 * @brief  PfcDomain Class Header
 * @author aics
 */

#include "PfcTextParser.h"
  
/** proc.pfc ファイルの Domain */
class CPfcDomain {

public:

  double m_globalOrigin[3];             ///<計算空間の起点座標
  double m_globalRegion[3];             ///<計算空間の各軸方向の長さ
  int    m_globalVoxel[3];              ///<計算領域全体のボクセル数
  int    m_globalDivision[3];           ///<計算領域の分割数

  /** コンストラクタ **/
  CPfcDomain();

  /** 
  * @brief コンストラクタ 
  * @param [in] globalOrigin    起点座標
  * @param [in] globalRegion    各軸方向の長さ
  * @param [in] globalVoxel     ボクセル数
  * @param [in] globalDivision  分割数
  */ 
  CPfcDomain(
             const double* globalOrigin, 
             const double* globalRegion, 
             const int*    globalVoxel, 
             const int*    globalDivision
            );

  /** デストラクタ **/
  ~CPfcDomain();

  /**
   * @brief read Domain(proc.dfi)
   * @param [in]   tpCntl  PfcTextParserクラス 
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Read(CPfcTextParser tpCntl);

  /**
   * @brief DFIファイル:Domainを出力する
   * @param [in] fp         ファイルポインタ
   * @param [in] tab        インデント
   * @return true:出力成功 false:出力失敗
   */
  PFC::E_PFC_ERRORCODE
  Write(FILE* fp, 
        const unsigned tab);

};

#endif // _PFC_DOMAIN_H_
