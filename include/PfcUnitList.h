#ifndef _PFC_UNITLIST_H_
#define _PFC_UNITLIST_H_


/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcUnitList.h
 * @brief  CPfcPfcUnitList Class Header
 * @author aics
 */

#include "cio_Define.h"
#include "cio_TextParser.h"
#include "cio_Unit.h"


/** index.pfc ファイルの TimeSlice */
//   CIOlib のcio_Unitを派生させて使う
//   ファイルフォーマットは同一
class CPfcUnitList : public cio_Unit {

public:

  /** コンストラクタ **/
  CPfcUnitList();

  /** デストラクタ **/
  ~CPfcUnitList();


};

#endif // _PFC_UNITLIST_H_
