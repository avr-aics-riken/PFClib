#ifndef _PFC_CIO_DFI_UTIL_H_
#define _PFC_CIO_DFI_UTIL_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcDfiUtil.h
 * @brief  CPfcDfiUtil Class Header  PFClib用 CIO Dfiファイルのユーティリティクラス
 * @author aics
 */

#include "PfcDefine.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <typeinfo>
#include <string>
#include <vector>

#include "cio_PathUtil.h"
#include "cio_TextParser.h"
#include "cio_TimeSlice.h"
#include "cio_Domain.h"
#include "cio_FilePath.h"


/** PFC CIO Dfi utility class */
class CPfcCioDfiUtil {

public:

  /**
   * @brief DFI Domain情報取得
   * @param[in]  dfiFilePath  DFIファイルパス
   * @param[out] domain       ドメイン情報
   * @return     終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GetDfiDomain( 
                const std::string&   dfiFilePath,
                cio_Domain&          domain
             );

  /**
   * @brief DFI タイムステップ リスト 情報取得
   * @param[in]  dfiFilePath    DFIファイルパス
   * @param[out] timeStepList   タイムステップ リスト
   * @return     終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GetDfiTimeStepList( 
                const std::string&   dfiFilePath,
                vector<int>&         timeStepList
             );

                        
protected :

  /**
   * @brief 内部ルーチン: DFI Dfi 情報取得
   * @param[in]  dfiFilePath    DFIファイルパス
   * @param[out] domain        ドメイン情報
   * @param[out] timeSlice     時系列データ
   * @return     終了コード
   */
  static PFC::E_PFC_ERRORCODE
  GetDfiInfo ( 
                const std::string&   dfiFilePath,
                cio_Domain&          domain,
                cio_TimeSlice&       timeSlice
             );

};

#endif // _PFC_CIO_DFI_UTIL_H_
