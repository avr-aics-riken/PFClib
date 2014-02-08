#ifndef _PFC_COMPRESS_INFO_H_
#define _PFC_COMPRESS_INFO_H_


/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompressInfo.h
 * @brief  CPfcCompressInfo Class Header
 * @author aics
 */


/** index.pfc ファイルの CompressInfo */
class CPfcCompressInfo {

public:
 
  std::string           m_compressFormat;   // 圧縮形式
  double                m_compressError;    // 誤差率(%)
  int                   m_calculatedLayer;  // 計算レイヤー数
  std::string           m_version;          // 圧縮バージョン
  int                   m_startStep;        // 開始ステップ
  int                   m_endStep;          // 終了ステップ


  /** コンストラクタ **/
  CPfcCompressInfo();

  /** 
   * @brief コンストラクタ 
   * @param [in] compressFormat    圧縮形式
   * @param [in] compressError     誤差率(%)
   * @param [in] calculatedLayer   計算レイヤー数
   * @param [in] version           圧縮バージョン
   * @param [in] startStep         開始step数
   * @param [in] endStep           終了step数
   */
  CPfcCompressInfo(
            const std::string&      compressFormat,
            const double            compressError,
            const int               calculatedLayer, 
            const std::string&      version,
            const int               startStep, 
            const int               endStep
        );

  /** デストラクタ **/
  ~CPfcCompressInfo();

public:

  /**
   * @brief read CompressInfo(index.pfc)
   * @param [in]   tpCntl  PfcTextParserクラス 
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Read( CPfcTextParser tpCntl); 

  /**
   * @brief write CompressInfo(index.pfc)
   * @param [in]  fp      ファイルポインタ
   * @param [in]  tab     インデント
   * @return error code
   */
  PFC::E_PFC_ERRORCODE
  Write(FILE* fp, 
        const unsigned tab); 

};

#endif // _PFC_COMPRESS_INFO_H_
