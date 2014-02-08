/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcRegion.cpp
 * @brief  PfcRegion Class
 * @author aics
 */

#include "PfcRegion.h"


// #################################################################
// コンストラクタ
CPfcRegion::CPfcRegion()
{

  m_regionID = 0;

  for(int i=0; i<3; i++) {
    m_voxelSize[i]=0;
    m_headIndex[i]=0;
    m_tailIndex[i]=0;
  }

}


// #################################################################
// デストラクタ
CPfcRegion::~CPfcRegion()
{

}

// #################################################################
// proc.pfcファイル:Region要素を読み込む
PFC::E_PFC_ERRORCODE
CPfcRegion::Read( CPfcTextParser tpCntl,
                  std::string label_leaf )
{

  std::string str;
  std::string label;
  int ct;
  int iv[3];

  //ID
  label = label_leaf + "/ID";
  if ( !(tpCntl.GetValue(label, &ct )) ) {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_REGION_ID;
  }
  else {
    m_regionID= ct;
  }

  //VoxelSize
  label = label_leaf + "/VoxelSize";
  for (int n=0; n<3; n++) iv[n]=0.0;
  if ( !(tpCntl.GetVector(label, iv, 3 )) ) 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_REGION_VOXELSIZE;
  }
  m_voxelSize[0]=iv[0];
  m_voxelSize[1]=iv[1];
  m_voxelSize[2]=iv[2];

  //HeadIndex
  label = label_leaf + "/HeadIndex";
  for (int n=0; n<3; n++) iv[n]=0.0;
  if ( !(tpCntl.GetVector(label, iv, 3 )) ) 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_REGION_HEADINDEX;
  }
  m_headIndex[0]=iv[0];
  m_headIndex[1]=iv[1];
  m_headIndex[2]=iv[2];

  //TailIndex
  label = label_leaf + "/TailIndex";
  for (int n=0; n<3; n++) iv[n]=0.0;
  if ( !(tpCntl.GetVector(label, iv, 3 )) ) 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_PFC_REGION_TAILINDEX;
  }
  m_tailIndex[0]=iv[0];
  m_tailIndex[1]=iv[1];
  m_tailIndex[2]=iv[2];

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// proc.pfcファイル:Region要素を出力する
PFC::E_PFC_ERRORCODE
CPfcRegion::Write(FILE* fp, 
                const unsigned tab) 
{

    _PFC_WRITE_TAB(fp, tab);
    fprintf(fp, "ID        = %d\n", m_regionID);

    _PFC_WRITE_TAB(fp, tab);
    fprintf(fp, "VoxelSize = (%d, %d, %d)\n", m_voxelSize[0],
                                              m_voxelSize[1],
                                              m_voxelSize[2]);

    _PFC_WRITE_TAB(fp, tab);
    fprintf(fp, "HeadIndex = (%d, %d, %d)\n", m_headIndex[0],
                                              m_headIndex[1],
                                              m_headIndex[2]);

    _PFC_WRITE_TAB(fp, tab);
    fprintf(fp, "TailIndex = (%d, %d, %d)\n", m_tailIndex[0],
                                              m_tailIndex[1],
                                              m_tailIndex[2]);

  return PFC::E_PFC_SUCCESS;

}
