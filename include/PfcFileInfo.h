#ifndef _PFC_FILEINFO_H_
#define _PFC_FILEINFO_H_


/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcFileInfo.h
 * @brief  CPfcFileInfo Class Header
 * @author aics
 */

#include "PfcTextParser.h"

/** index.pfc ファイルの FileInfo */
class CPfcFileInfo {

public:
 
  std::string           m_dirPath;      ///<フィールデータの存在するディレクトリパス
                                        ///< index.dfiからの相対パスまたは絶対パス
  std::string           m_prefix;       // ベースファイル名  （属性名）
  std::string           m_fileFormat;   ///<ファイルフォーマット "pod"
  int                   m_guideCell;    ///<仮想セルの数         (0固定）
  PFC::E_PFC_DTYPE      m_dataType;     ///<配列のデータタイプ  （PFC::E_PFC_FLOAT64）
  PFC::E_PFC_ENDIANTYPE m_endianType;   ///<エンディアンタイプ  E_PFC_LITTLE / E_PFC_BIG
  PFC::E_PFC_ARRAYSHAPE m_arrayShape;   // 配列形状    E_PFC_IJKN / E_PFC_NIJK
  int                   m_numComponent; // 成分数


  /** コンストラクタ **/
  CPfcFileInfo();

  /** 
   * @brief コンストラクタ 
   * @param [in] dirPath          フィールデータの存在するディレクトリパス
   * @param [in] prefix           ベースファイル名  （属性名）
   * @param [in] fileFormat       ファイルフォーマット
   * @param [in] guideCell        仮想セルの数
   * @param [in] dataType         配列のデータタイプ
   * @param [in] endianType       エンディアンタイプ
   * @param [in] arrayShape       配列形状
   * @param [in] numComponent     成分数
   */
  CPfcFileInfo(const std::string&           dirPath,
               const std::string&           prefix,
               const std::string&           fileFormat, 
               const int                    guideCell,
               const PFC::E_PFC_DTYPE       dataType, 
               const PFC::E_PFC_ENDIANTYPE  endianType,
               const PFC::E_PFC_ARRAYSHAPE  arrayShape, 
               const int numComponent);

  /** デストラクタ **/
  ~CPfcFileInfo();

public:

  /**
   * @brief read FileInfo(index.pfc)
   * @param [in]   tpCntl  PfcTextParserクラス 
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Read( CPfcTextParser tpCntl); 

  /**
   * @brief write FileInfo(index.pfc)
   * @param [in]  fp      ファイルポインタ
   * @param [in]  tab     インデント
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Write(FILE* fp, 
        const unsigned tab); 

};

#endif // _PFC_FILEINFO_H_
