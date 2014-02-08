/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcFileInfo.cpp
 * @brief  CPfcFileInfo Class
 * @author aics
 */

#include "PfcDefine.h"
#include "PfcFileInfo.h"


// #################################################################
// コンストラクタ
CPfcFileInfo::CPfcFileInfo()
{
  m_dirPath      = "";
  m_prefix       = "";
  m_fileFormat   = "";
  m_guideCell    = 0;
  m_dataType     = PFC::E_PFC_DTYPE_UNKNOWN;
  m_endianType   = PFC::E_PFC_ENDIANTYPE_UNKNOWN;
  m_arrayShape   = PFC::E_PFC_ARRAYSHAPE_UNKNOWN;
  m_numComponent = 0;
}

// #################################################################
// コンストラクタ
CPfcFileInfo::CPfcFileInfo(
               const std::string&           dirPath,
               const std::string&           prefix,
               const std::string&           fileFormat, 
               const int                    guideCell,
               const PFC::E_PFC_DTYPE       dataType, 
               const PFC::E_PFC_ENDIANTYPE  endianType,
               const PFC::E_PFC_ARRAYSHAPE  arrayShape, 
               const int                    numComponent
             )
{
  m_dirPath      = dirPath;
  m_prefix       = prefix;
  m_fileFormat   = fileFormat;
  m_guideCell    = guideCell;
  m_dataType     = dataType;
  m_endianType   = endianType;
  m_arrayShape   = arrayShape;
  m_numComponent = numComponent;
}

// デストラクタ
CPfcFileInfo::~CPfcFileInfo()
{

}


// #################################################################
// FileInfo 読込み関数
PFC::E_PFC_ERRORCODE
CPfcFileInfo::Read( CPfcTextParser tpCntl) 
{

  std::string str;
  std::string label,label_base,label_leaf,label_leaf_leaf;
  int ct;

  //Directorypath
  label = "/FileInfo/DirectoryPath";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_DIRECTORYPATH;
  }
  m_dirPath=str;

  //Prefix
  label = "/FileInfo/Prefix";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_PREFIX;
  }
  m_prefix=str;

  //FileFormat
  label = "/FileInfo/FileFormat";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_FILEFORMAT;
  }
  m_fileFormat = str;


  //GuidCell
  label = "/FileInfo/GuideCell";
  if ( !(tpCntl.GetValue(label, &ct )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_GUIDECELL;
  }
  m_guideCell=ct;

  //DataType
  label = "/FileInfo/DataType";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_DATATYPE;
  }
  if( !strcasecmp(str.c_str(),"Float64" ) ) {
    m_dataType = PFC::E_PFC_FLOAT64;
  }else {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",str.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_DATATYPE;
  } 

  //Endian
  label = "/FileInfo/Endian";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_ENDIAN;
  }
  if( !strcasecmp(str.c_str(),"little" ) ) {
    m_endianType=PFC::E_PFC_LITTLE;
  }else if( !strcasecmp(str.c_str(),"big" ) ) {
    m_endianType=PFC::E_PFC_BIG;
  }else {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",str.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_ENDIAN;
  } 

  //ArrayShape  
  label = "/FileInfo/ArrayShape";
  if ( !(tpCntl.GetValue(label, &str )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_ARRAYSHAPE;
  }
  if( !strcasecmp(str.c_str(),"ijkn" ) ) {
    m_arrayShape = PFC::E_PFC_IJKN;
  } else if( !strcasecmp(str.c_str(),"nijk" ) ) {
    m_arrayShape = PFC::E_PFC_NIJK;
  }else {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",str.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_ARRAYSHAPE;
  }

  //Componet  
  label = "/FileInfo/Component";
  if ( !(tpCntl.GetValue(label, &ct )) )
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_COMPONENT;
  }
  m_numComponent = ct;


  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// FileInfo要素を出力する
PFC::E_PFC_ERRORCODE
CPfcFileInfo::Write(FILE* fp, 
                    const unsigned tab) 
{

  fprintf(fp, "FileInfo {\n");
  fprintf(fp, "\n");

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "DirectoryPath      = \"%s\"\n", m_dirPath.c_str());

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "Prefix             = \"%s\"\n", m_prefix.c_str());

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "FileFormat         = \"%s\"\n",m_fileFormat.c_str());

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "GuideCell          = %d\n", m_guideCell);

  _PFC_WRITE_TAB(fp, tab+1);
  if( m_dataType == PFC::E_PFC_FLOAT64 ) {
    fprintf(fp, "DataType          = \"Float64\"\n");
  } else {
    PFC_PRINT("\tPFC Parsing error : fail to set '%s'\n","DataType");
    return PFC::E_PFC_ERROR_WRITE_PFC_DATATYPE;
  }

  _PFC_WRITE_TAB(fp, tab+1);
  if( m_endianType == PFC::E_PFC_LITTLE) {
    fprintf(fp, "Endian             = \"little\"\n");
  } else {
    fprintf(fp, "Endian             = \"big\"\n");
  }

  _PFC_WRITE_TAB(fp, tab+1);
  if( m_arrayShape == PFC::E_PFC_IJKN ) {
    fprintf(fp, "ArrayShape         = \"ijkn\"\n");
  } else {
    fprintf(fp, "ArrayShape         = \"nijk\"\n");
  }

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "Component          = %d\n",m_numComponent);


  fprintf(fp, "\n");
  fprintf(fp, "}\n");
  fprintf(fp, "\n");

  return PFC::E_PFC_SUCCESS;

}

