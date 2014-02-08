#ifndef _PFC_COMPRESS_CMD_ELM_
#define _PFC_COMPRESS_CMD_ELM_CNTL_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompressCmdElm.h
 * @brief  CPfcCompressCmdElm Class Header
 * @author aics
 */

#include "PfcDefine.h"
#include <string>
#include "PfcTextParser.h"

/** pfc_cntl ファイルの 圧縮制御データ要素 */
class CPfcCompressCmdElm {

public :
  
  std::string   m_dfiFilePath;          ///<CIOのDFIファイル
  std::string   m_outDirPath;           ///<フィールドデータの出力ディレクトリパス

  std::string   m_compressFormat;       // 圧縮形式
  double        m_compressError;        // 誤差率（指定した誤差率）
                                        //   指定なしの場合、デフォルト値使用

  int m_startStep;                      ///<開始タイムステップ
                                        //    == -1 dfiファイルに含まれる全タイムステップ
                                        //   pfc_cntlファイルに指定なき場合、全タイムステップとみなす
  int m_endStep;                        ///<終了タイムステップ
                                        //    == -1 dfiファイルに含まれる全タイムステップ
                                        //   pfc_cntlファイルに指定なき場合、全タイムステップとみなす
  std::string   m_procFileSave;         // proc.pfcファイル出力の有無
                                        //  オプションは将来複数の可能性あり
  std::string   m_optSave;              // オプション（圧縮制御データ 指定値）
                                        //  オプションは将来複数の可能性あり
                                        //  -> m_optFlagsに反映させる
                                        //      m_optFlags = XXX | YYY | ZZZ で作成
  int           m_optFlags;             // オプション フラグ  （マージした値）
                                        //  == 0 オプションなし

public :

  /** コンストラクタ */
  CPfcCompressCmdElm();

  /** デストラクタ */
  ~CPfcCompressCmdElm();

  /**
  * @brief ItemCntl要素を読込む(pfc_cntl)
  * @param [in]  tpCntl     CPfcTextParserクラス
  * @param [in]  label_leaf ベースとなる名前（"/PfcCompressCntl/ItemCntl")   
  * @return error code
  */  
  PFC::E_PFC_ERRORCODE
  ReadElm(CPfcTextParser tpCntl, 
       const std::string& label_leaf);

protected :
#if 0
  /**
  * @brief Dfi GlovalDivision取得
  * @param [in]   dfiFilePath  DFIファイルパス
  * @param [out]  gDiv         DFIの計算領域の分割数
  */
  PFC::E_PFC_ERRORCODE
  GetDfiGdiv( const std::string& dfiFilePath, int gDiv[3] );
#endif

};
#endif // _PFC_COMPRESS_CMD_ELM_
