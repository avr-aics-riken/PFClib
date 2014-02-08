/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcFilePath.cpp
 * @brief  CPfcFilePath Class
 * @author aics
 */

#include "PfcDefine.h"
#include "PfcFilePath.h"


// #################################################################
// コンストラクタ
CPfcFilePath::CPfcFilePath()
{
  m_dfiPath      = "";
  m_procPath     = "";
}

// #################################################################
// コンストラクタ
CPfcFilePath::CPfcFilePath(
                    const std::string&              dfiPath,
                    const std::string&              procPath
             )
{
  m_dfiPath      = dfiPath;
  m_procPath     = procPath;
}

// デストラクタ
CPfcFilePath::~CPfcFilePath()
{

}


// #################################################################
// FilePath 読込み関数
PFC::E_PFC_ERRORCODE
CPfcFilePath::Read( CPfcTextParser tpCntl) 
{

  std::string str;
  std::string label,label_base,label_leaf,label_leaf_leaf;

  //DfiPath
  label = "/FilePath/DfiPath";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_DFIPATH;
  }
  m_dfiPath=str;

  //PfcProcess
  label = "/FilePath/PfcProcess";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_PFCPROCESS;
  }
  m_procPath=str;

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// FilePath要素を出力する
PFC::E_PFC_ERRORCODE
CPfcFilePath::Write(FILE* fp, 
                    const unsigned tab) 
{

  fprintf(fp, "FilePath {\n");
  fprintf(fp, "\n");

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "DfiPath            = \"%s\"\n", m_dfiPath.c_str());

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "PfcProcess         = \"%s\"\n", m_procPath.c_str());


  fprintf(fp, "\n");
  fprintf(fp, "}\n");
  fprintf(fp, "\n");

  return PFC::E_PFC_SUCCESS;

}

