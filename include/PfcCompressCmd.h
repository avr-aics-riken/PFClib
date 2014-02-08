#ifndef _PFC_COMPRESS_CMD_H_
#define _PFC_COMPRESS_CMD_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompressCmd.h
 * @brief  CPfcCompressCmd Class Header
 * @author aics
 */

#include "PfcDefine.h"
#include <string>
#include "PfcCompressCmdElm.h"
#include "PfcTextParser.h"


/** PFC Compress command main class */
class CPfcCompressCmd {

public:

protected :

  std::string m_compressCntlPath;     ///< 圧縮制御情報ファイルのディレクトリパス

  int         m_domainDivision[3];    ///<計算領域の分割数
                                      //  pfc_cntlファイルに指定なき場合、
                                     //  dfiファイルのGlobalDivisionを設定する

  vector<CPfcCompressCmdElm> m_cmdList;     ///< 圧縮制御情報リスト


public:
  /** コンストラクタ */
  CPfcCompressCmd( const std::string& compressCntlPath );
  
  /**　デストラクタ */
  ~CPfcCompressCmd();

  /**
   * @brief 圧縮実行
   * @return エラーコード
   */
  PFC::E_PFC_ERRORCODE
  Execute( void );

protected :
  /**
   * @brief 圧縮制御情報ファイル読み込み
   * @return エラーコード
   */
  PFC::E_PFC_ERRORCODE
  ReadCompressCntl( void );

};


#endif // _PFC_COMPRESS_CMD_H_
