/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcDomain.cpp
 * @brief  PfcDomain Class
 * @author aics
 */

#include "PfcDomain.h"


// #################################################################
// コンストラクタ
CPfcDomain::CPfcDomain()
{

  for(int i=0; i<3; i++) m_globalOrigin[i]=0.0;
  for(int i=0; i<3; i++) m_globalRegion[i]=0.0;
  for(int i=0; i<3; i++) m_globalVoxel[i]=0;
  for(int i=0; i<3; i++) m_globalDivision[i]=0;

}

// #################################################################
// コンストラクタ
CPfcDomain::CPfcDomain(
                       const double* globalOrigin, 
                       const double* globalRegion, 
                       const int*    globalVoxel,
                       const int*    globalDivision
                    )
{
  m_globalOrigin[0] = globalOrigin[0];
  m_globalOrigin[1] = globalOrigin[1];
  m_globalOrigin[2] = globalOrigin[2];

  m_globalRegion[0] = globalRegion[0];
  m_globalRegion[1] = globalRegion[1];
  m_globalRegion[2] = globalRegion[2];

  m_globalVoxel[0] = globalVoxel[0];
  m_globalVoxel[1] = globalVoxel[1];
  m_globalVoxel[2] = globalVoxel[2];

  m_globalDivision[0] = globalDivision[0];
  m_globalDivision[1] = globalDivision[1];
  m_globalDivision[2] = globalDivision[2];
}

// #################################################################
// デストラクタ
CPfcDomain::~CPfcDomain()
{

}

// #################################################################
// Domain の読込み関数
PFC::E_PFC_ERRORCODE
CPfcDomain::Read(CPfcTextParser tpCntl) 
{

  std::string str;
  std::string label;
  double v[3];
  int iv[3];

  //GlobalOrign
  label = "/Domain/GlobalOrigin";
  for (int n=0; n<3; n++) v[n]=0.0;
  if ( !(tpCntl.GetVector(label, v, 3 )) ) 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_GLOBALORIGIN;
  }
  m_globalOrigin[0]=v[0];
  m_globalOrigin[1]=v[1];
  m_globalOrigin[2]=v[2];

  //GlobalRegion
  label = "/Domain/GlobalRegion";
  for (int n=0; n<3; n++) v[n]=0.0;
  if ( !(tpCntl.GetVector(label, v, 3 )) ) 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_GLOBALREGION;
  }
  m_globalRegion[0]=v[0];
  m_globalRegion[1]=v[1];
  m_globalRegion[2]=v[2];

  //Global_Voxel
  label = "/Domain/GlobalVoxel";
  for (int n=0; n<3; n++) iv[n]=0;
  if ( !(tpCntl.GetVector(label, iv, 3 )) ) 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_GLOBALVOXEL;
  }
  m_globalVoxel[0]=iv[0];
  m_globalVoxel[1]=iv[1];
  m_globalVoxel[2]=iv[2];

  //Global_Division
  label = "/Domain/GlobalDivision";
  for (int n=0; n<3; n++) iv[n]=0;
  if ( !(tpCntl.GetVector(label, iv, 3 )) ) 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_GLOBALDIVISION;
  }
  m_globalDivision[0]=iv[0];
  m_globalDivision[1]=iv[1];
  m_globalDivision[2]=iv[2];

  return PFC::E_PFC_SUCCESS;

}

// #################################################################
// PFCファイル:Domain要素を出力する
PFC::E_PFC_ERRORCODE
CPfcDomain::Write(FILE* fp, 
                  const unsigned tab)
{

  fprintf(fp, "Domain {\n");
  fprintf(fp, "\n");  

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "GlobalOrigin        = (%e, %e, %e)\n",
          m_globalOrigin[0],
          m_globalOrigin[1],
          m_globalOrigin[2]);

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "GlobalRegion        = (%e, %e, %e)\n",
          m_globalRegion[0],
          m_globalRegion[1],
          m_globalRegion[2]);

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "GlobalVoxel         = (%d, %d, %d)\n",
          m_globalVoxel[0],
          m_globalVoxel[1],
          m_globalVoxel[2]);  

  _PFC_WRITE_TAB(fp, tab+1);
  fprintf(fp, "GlobalDivision      = (%d, %d, %d)\n",
          m_globalDivision[0],
          m_globalDivision[1],
          m_globalDivision[2]);

  fprintf(fp, "\n");
  fprintf(fp, "}\n");
  fprintf(fp, "\n");

  return PFC::E_PFC_SUCCESS;

}
