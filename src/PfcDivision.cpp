/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcDivision.cpp
 * @brief  PfcDivision Class
 * @author aics
 */

#include "PfcDivision.h"
#include "PfcFunction.h"

// #################################################################
// コンストラクタ
CPfcDivision::CPfcDivision()
{
  m_numRegion   = 0;
  m_MinMax      = NULL;
  m_MinMax_data = NULL;

}

// #################################################################
// デストラクタ
CPfcDivision::~CPfcDivision()
{
  if( m_MinMax != NULL ) delete[] m_MinMax;
  if( m_MinMax_data != NULL ) delete[] m_MinMax_data;
}

// #################################################################
// proc.pfcファイル:Division要素を読み込む
PFC::E_PFC_ERRORCODE
CPfcDivision::Read(CPfcTextParser tpCntl)
{

  std::string str;
  std::string label_base,label_leaf;
  int nnode=0;
  PFC::E_PFC_ERRORCODE iret;  ///<リターンコード

  CPfcRegion region;

  //Division
  nnode=0;
  label_base = "/Division";
  if ( tpCntl.chkNode(label_base) )  //nodeがあれば
  {
    nnode = tpCntl.countLabels(label_base);
  }

  for (int i=0; i<nnode; i++) {

    if(!tpCntl.GetNodeStr(label_base,i+1,&str))
    {
      PFC_PRINT("\tPFC Parsing error : No Elem name\n");
      return PFC::E_PFC_ERROR_READ_PFC_NO_REGION;
    }
    if( strcasecmp(str.substr(0,6).c_str(), "Region") ) continue;
    label_leaf=label_base+"/"+str;

    /** Regionの読込み */
    iret = region.Read(tpCntl, label_leaf);
    if( iret == PFC::E_PFC_SUCCESS ) {
      m_regionList.push_back(region); 
    } else {
      return iret;
    }

  }
  
  m_numRegion = m_regionList.size();

  return PFC::E_PFC_SUCCESS;

}

// #################################################################
// 読込み領域リストの取得
PFC::E_PFC_ERRORCODE 
CPfcDivision::CheckReadRegion(
                           const int head[3],
                           const int tail[3],
                           vector<int> &regionIdList)
{
  PFC::E_PFC_ERRORCODE ret; 
  regionIdList.clear();

  //proc.pfcにDivision/Region[@]がない処理
  if( m_numRegion == 0 ) {
    PFC_PRINT("\tPFC No Region\n");
    return PFC::E_PFC_SUCCESS;
  }

  //m_MinMaxが未定義（proc.pfcにDivision/Region[@]がある場合）
  if( m_MinMax == NULL ) {
    CreateMinMax(); // MinMax領域設定
  }

  //MinMaxチェック
  for(int i=0; i<m_numRegion; i++) {
    ret = CPfcFunction::CheckLapMinMax(
                            &m_MinMax[i][0], &m_MinMax[i][3],
                            head,            tail
                        );
    if( ret == PFC::E_PFC_SUCCESS ) {
      regionIdList.push_back(i);
    }
  }
  
  if( regionIdList.size() == 0 ) {
    return PFC::E_PFC_ERROR;
  }

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 読込み領域IDの取得
PFC::E_PFC_ERRORCODE 
CPfcDivision::CheckReadRegion(
                           const int ix,    // [in]  X インデックス
                           const int iy,    // [in]  Y インデックス
                           const int iz,    // [in]  Z インデックス
                           int&  regionId   // [out] 読込み領域ID
                       )
{
  PFC::E_PFC_ERRORCODE ret;
  regionId = -1;

  //proc.pfcにDivision/Region[@]がない処理
  if( m_numRegion == 0 ) {
    PFC_PRINT("\tPFC No Region\n");
    return PFC::E_PFC_SUCCESS;
  }

  //m_MinMaxが未定義（proc.pfcにDivision/Region[@]がある場合）
  if( m_MinMax == NULL ) {
    CreateMinMax(); // MinMax領域設定
  }

  //MinMaxチェック
  for(int i=0; i<m_numRegion; i++) {
    ret = CPfcFunction::CheckPntInMinMax(
                            &m_MinMax[i][0], &m_MinMax[i][3],
                            ix, iy, iz
                        );
    if( ret == PFC::E_PFC_SUCCESS ) {
      regionId = i;
      break;
    }
  }
   
  if( regionId == -1 ) {
    return PFC::E_PFC_ERROR;
  }

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// proc.pfcファイル: Division要素を出力する
PFC::E_PFC_ERRORCODE
CPfcDivision::Write(FILE* fp, 
                   const unsigned tab) 
{

  fprintf(fp, "Division {\n");
  fprintf(fp, "\n");

  CPfcRegion region;

  for(int i=0; i<m_regionList.size(); i++) {
    _PFC_WRITE_TAB(fp, tab+1);
    fprintf(fp, "Region[@] {\n");
    fprintf(fp, "\n");

    region = m_regionList[i];

    //Region要素の出力
    if( region.Write(fp,tab+2) != PFC::E_PFC_SUCCESS ) return PFC::E_PFC_ERROR;

    fprintf(fp, "\n");
    _PFC_WRITE_TAB(fp, tab+1);
    fprintf(fp, "}\n");
  }

  fprintf(fp, "\n");
  fprintf(fp, "}\n");

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// MinMax領域設定
void
CPfcDivision::CreateMinMax( void )
{
   // 領域アロケーション
   CPfcFunction::Alloc2D( m_numRegion, 6, m_MinMax_data, m_MinMax );
   
   // MinMax設定
   for(int i=0; i<m_numRegion; i++) {
     m_MinMax[i][0] = m_regionList[i].m_headIndex[0];
     m_MinMax[i][1] = m_regionList[i].m_headIndex[1];
     m_MinMax[i][2] = m_regionList[i].m_headIndex[2];
     m_MinMax[i][3] = m_regionList[i].m_tailIndex[0];
     m_MinMax[i][4] = m_regionList[i].m_tailIndex[1];
     m_MinMax[i][5] = m_regionList[i].m_tailIndex[2];
   }
}

