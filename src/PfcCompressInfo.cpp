/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompressInfo.cpp
 * @brief  CPfcCompressInfo Class
 * @author aics
 */

#include "PfcDefine.h"
#include "PfcTextParser.h"
#include "PfcCompressInfo.h"


// #################################################################
// コンストラクタ
CPfcCompressInfo::CPfcCompressInfo()
{
  m_compressFormat  = "";
  m_compressError   = 0.0;
  m_calculatedLayer = 0;
  m_version         = "";
  m_startStep       = -1;
  m_endStep         = -1;
}

// #################################################################
// コンストラクタ
CPfcCompressInfo::CPfcCompressInfo(
            const std::string&      compressFormat,
            const double            compressError,
            const int               calculatedLayer, 
            const std::string&      version,
            const int               startStep, 
            const int               endStep
         )
{
  m_compressFormat  = compressFormat;
  m_compressError   = compressError;
  m_calculatedLayer = calculatedLayer;
  m_version         = version;
  m_startStep       = startStep;
  m_endStep         = endStep;
}

// デストラクタ
CPfcCompressInfo::~CPfcCompressInfo()
{

}


// #################################################################
// CompressInfo 読込み関数
PFC::E_PFC_ERRORCODE
CPfcCompressInfo::Read( CPfcTextParser tpCntl) 
{

  std::string str;
  std::string label,label_base,label_leaf,label_leaf_leaf;
  double dt;
  int ct;

  //CompressFormat
  label = "/CompressInfo/CompressFormat";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_COMPRESSFORMAT;
  }
  m_compressFormat=str;

  //CompressError
  label = "/CompressInfo/CompressError";
  if ( !(tpCntl.GetValue(label, &dt )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_COMPRESSERROR;
  }
  m_compressError=dt;

  //CalculatedLayer
  label = "/CompressInfo/CalculatedLayer";
  if ( !(tpCntl.GetValue(label, &ct )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_CALCULATEDLAYER;
  }
  m_calculatedLayer = ct;


  //Version
  label = "/CompressInfo/Version";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_VERSION;
  }
  m_version = str;

  //StartStep
  label = "/CompressInfo/StartStep";
  if ( !(tpCntl.GetValue(label, &ct )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_STARTSTEP;
  }
  m_startStep = ct;

  //EndStep
  label = "/CompressInfo/EndStep";
  if ( !(tpCntl.GetValue(label, &ct )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_ENDSTEP;
  }
  m_endStep = ct;


  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// CompressInfo要素を出力する
PFC::E_PFC_ERRORCODE
CPfcCompressInfo::Write(FILE* fp, 
                    const unsigned tab) 
{

  fprintf(fp, "CompressInfo {\n");
  fprintf(fp, "\n");

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "CompressFormat     = \"%s\"\n", m_compressFormat.c_str());

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "CompressError      = %6.3f\n", m_compressError);

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "CalculatedLayer    = %d\n",m_calculatedLayer);

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "Version            = \"%s\"\n", m_version.c_str());

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "StartStep          = %d\n",m_startStep);

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "EndStep            = %d\n",m_endStep);


  fprintf(fp, "\n");
  fprintf(fp, "}\n");
  fprintf(fp, "\n");

  return PFC::E_PFC_SUCCESS;
}

