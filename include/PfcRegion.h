#ifndef _PFC_REGION_H_
#define _PFC_REGION_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcRegion.h
 * @brief  PfcRegion Class Header
 * @author aics
 */

#include "PfcDefine.h"
#include "PfcTextParser.h"

/** proc.pfc ファイルの Region */
class CPfcRegion {

public:


  int m_regionID;                       ///<分割した領域のID
  int m_voxelSize[3];                   ///<ボクセルサイズ
  int m_headIndex[3];                   ///<始点インデックス
  int m_tailIndex[3];                   ///<終点インデックス

  /** コンストラクタ **/
  CPfcRegion();

  /** デストラクタ **/
  ~CPfcRegion();

  /**
   * @brief read Region(proc.pfc)
   * @param [in]   tpCntl  PfcTextParserクラス
   * @param [in]   label_leaf ベースとなる名前（"/Division/Region") 
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Read(CPfcTextParser tpCntl,
       std::string label_leaf);

  /**
   * @brief proc.pfcファイル: Region出力する
   * @param [in] fp       ファイルポインタ
   * @param [in] tab      インデント
   * @return true:出力成功 false:出力失敗
   */
  PFC::E_PFC_ERRORCODE
  Write(FILE* fp, 
        const unsigned tab);

};

#endif // _PFC_REGION_H_
