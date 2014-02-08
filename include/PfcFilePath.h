#ifndef _PFC_FILEPATH_H_
#define _PFC_FILEPATH_H_


/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcFilePath.h
 * @brief  CPfcFilePath Class Header
 * @author aics
 */

#include "PfcTextParser.h"


/** index.pfc ファイルの FilePath */
class CPfcFilePath {

public:
 
  std::string           m_dfiPath;      // CIO DFIファイルパス
  std::string           m_procPath;     // proc.pfc ファイルパス

  /** コンストラクタ **/
  CPfcFilePath();

  /** 
   * @brief コンストラクタ 
   * @param [in] dfiPath    CIO DFIファイルパス
   * @param [in] procPath   proc.pfc ファイルパス
   */
  CPfcFilePath(
                 const std::string&             dfiPath,
                 const std::string&             procPath
           );

  /** デストラクタ **/
  ~CPfcFilePath();

public:

  /**
   * @brief read FilePath(index.pfc)
   * @param [in]   tpCntl  PfcTextParserクラス 
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Read( CPfcTextParser tpCntl); 

  /**
   * @brief write FilePath(index.pfc)
   * @param [in]  fp      ファイルポインタ
   * @param [in]  tab     インデント
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Write(FILE* fp, 
        const unsigned tab); 

};

#endif // _PFC_FILEPATH_H_
