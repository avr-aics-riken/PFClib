/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompressCmdElm.cpp 
 * @brief  CPfcCompressCmdElm Class
 * @author aics
 */

#include "PfcCompressCmdElm.h"


// #################################################################
// コンストラクタ
CPfcCompressCmdElm::CPfcCompressCmdElm()
{
  m_dfiFilePath       = "";
  m_outDirPath        = "";
  m_compressFormat    = "";
  m_compressError     = D_PFC_COMPRESS_ERROR_DEFAULT;
  m_startStep         = 0;
  m_endStep           = 0;
  m_procFileSave      = "";
  m_optSave           = "";
  m_optFlags          = 0;
}


// #################################################################
// デストラクタ
CPfcCompressCmdElm::~CPfcCompressCmdElm()
{

}

// #################################################################
// 圧縮制御データ １要素の読込み
PFC::E_PFC_ERRORCODE
CPfcCompressCmdElm::ReadElm(CPfcTextParser tpCntl,
                        const std::string& label_leaf) 
{
  std::string str;
  std::string label,label_leaf_leaf;

  int ct;
  double dt;

  // InputDfiPath
  label = label_leaf + "/InputDfiPath";
  if ( tpCntl.GetValue(label, &str ) ) 
  {
     m_dfiFilePath=str;
  }
  else
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_CNTL_DFI_PATH;
  }

  // OutputDirectoryPath
  label = label_leaf + "/OutputDirectoryPath";
  if ( tpCntl.GetValue(label, &str ) ) 
  {
     m_outDirPath=str;
  }
  else
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_CNTL_OUTDIR_PATH;
  }

  // CompressFormat
  label = label_leaf + "/CompressFormat";
  if ( tpCntl.GetValue(label, &str ) ) 
  {
     m_compressFormat=str;
  }
  else 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_CNTL_COMPRESS_FMT;
  }

  // CompressError
  label = label_leaf + "/CompressError";
  if ( tpCntl.GetValue(label, &dt ) ) 
  {
    m_compressError = dt;
  }
  else
  {
    m_compressError = D_PFC_COMPRESS_ERROR_DEFAULT;
  }

  // 開始タイムステップ
  label = label_leaf + "/StartStep";
  if ( tpCntl.GetValue(label, &ct ) ) 
  {
    m_startStep = ct;
  }
  else
  {
    m_startStep = -1;
  }

  // 終了タイムステップ
  label = label_leaf + "/EndStep";
  if ( tpCntl.GetValue(label, &ct ) ) 
  {
    m_endStep = ct;
  }
  else
  {
    m_endStep = -1;
  }

  // ProcFileSave
  label = label_leaf + "/ProcFileSave";
  if ( tpCntl.GetValue(label, &str ) ) 
  {
     m_procFileSave=str;
  }
  else 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_CNTL_PROCFILE_SAVE;
  }

  // OptSave
  label = label_leaf + "/OptSave";
  if ( tpCntl.GetValue(label, &str ) ) 
  {
     m_optSave=str;
     if( m_optSave == "ON" ) {
       m_optFlags = m_optFlags | D_PFC_OPT_FLAG_SAVE;
     }
  }
  else 
  {
     m_optSave="";
  }

  return PFC::E_PFC_SUCCESS;
}

#if 0
// #################################################################
// Dfi GlovalDivision取得
PFC::E_PFC_ERRORCODE
CPfcCompressCmdElm::GetDfiGdiv( const std::string& dfiFilePath, int gDiv[3] )
{
  PFC::E_PFC_ERRORCODE ret;
  cio_Domain domain;
  
  ret = CPfcCioDfiUtil::GetDfiDomain( dfiFilePath, domain );
  if( !ret ) {
      PFC_PRINT("\tPFC Parsing error : No Elem name\n");
      return PFC::E_PFC_ERROR;
  }
  
    // GlobalDivision設定
  gDiv[0] = domain.GlobalDivision[0];
  gDiv[1] = domain.GlobalDivision[1];
  gDiv[2] = domain.GlobalDivision[2];

  return PFC::E_PFC_SUCCESS;

}
#endif

