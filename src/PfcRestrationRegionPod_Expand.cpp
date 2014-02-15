/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   CPfcRestrationRegionPod_Expand.cpp  分割領域 展開用
 * @brief  CCPfcRestrationRegionPod Class
 * @author aics
 */

#include <math.h>
#include "PfcRestrationRegionPod.h"
#include "PfcFunction.h"
#include "PfcPodFile.h"
#include "PfcCoeffTree.h"


// #################################################################
// 圧縮データ展開（領域内全情報,Load済みのデータから展開）
PFC::E_PFC_ERRORCODE
CPfcRestrationRegionPod::ExpandData(
              double*   v,          // [out] 展開後の出力領域 
                                    //       メモリは確保済みであること
                                    //       領域サイズ：head-tail間のボクセルサイズ*成分数
              const int stepID      // [in]  ステップID  (0起点）
           )
{
  PFC::E_PFC_ERRORCODE ret;
  int max_layer = CPfcFunction::GetPodMaxLayer( m_numStep );
  int num_base  = int_pow(2,max_layer);
  int mlayer    = max_layer - m_numCalculatedLayer;
  int rnum = 1;
  for(int i=0; i<=mlayer; i++ ) {
    rnum = rnum*2;
  }
  
  // rowNum : 検索する経路の数
  int rowNum  = num_base/rnum;
#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcRestrationRegionPod::ExpandData() Start\n");
  DEBUG_PRINT("       m_numStep           =%d\n",m_numStep);
  DEBUG_PRINT("       max_layer           =%d\n",max_layer);
  DEBUG_PRINT("       m_numCalculatedLayer=%d\n",m_numCalculatedLayer);
  DEBUG_PRINT("       num_base            =%d\n",num_base);
  DEBUG_PRINT("       mlayer              =%d\n",mlayer);
  DEBUG_PRINT("       rowNum              =%d\n",rowNum);
#endif
  
  int nsize_region = m_numComponent*m_numSize;

  // 経路毎の情報
  double* coef       = new double[rowNum];
  int*    base_index = new int[rowNum];
  int*    row_st     = new int[rowNum];
  int*    row_size   = new int[rowNum];

  // 経路情報取得
#ifdef USE_PMLIB
  PfcPerfMon::Start(t271_r);
#endif
  ret = GetExpandRowInfo(
                stepID,         // [in]  ステップID
                rowNum,         // [in]  経路の数
                coef,           // [out] 経路毎の係数
                base_index,     // [out] 経路毎の基底のインデックス
                row_st,         // [out] 経路毎の開始位置  (0起点）
                row_size        // [out] 経路毎のサイズ
            );
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t271_r);
#endif
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("Error  GetExpandRowInfo()\n");
    delete[] coef;
    delete[] base_index;
    delete[] row_st;
    delete[] row_size;
    return ret;
  }
#ifdef DEBUG_PFC
  DEBUG_PRINT("    GetExpandRowInfo()\n");
  DEBUG_PRINT("       stepID=%d\n",stepID);
  DEBUG_PRINT("       rowNum=%d\n",rowNum);
  for(int i=0; i<rowNum; i++) {
    DEBUG_PRINT("       i=%d coef=%lf base_index=%d row_st=%d row_size=%d\n",
                      i,coef[i],base_index[i],row_st[i],row_size[i] );
  }
#endif

  //------------------------------------------//
  //        出力域に設定                      //
  //------------------------------------------//
  int  nsize = 0;
  // 経路の数ループ
#ifdef USE_PMLIB
  PfcPerfMon::Start(t273_r);
#endif
  for(int i = 0; i < rowNum; i++){
    int b_index   = base_index[i];
    for(int k = 0; k < row_size[i]; k++) {
      v[nsize] = coef[i]*m_pIndexBase[b_index][k];
      nsize++;
    }
  }
#ifdef USE_PMLIB
  PfcPerfMon::Stop(t273_r);
#endif
  
  if( nsize != nsize_region ) {
    PFC_PRINT("Data size error nsize=%d nsize_region=5d\n", nsize, nsize_region);
    ret = PFC::E_PFC_ERROR;
  }

  delete[] coef;
  delete[] base_index;
  delete[] row_st;
  delete[] row_size;

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcRestrationRegionPod::ExpandData() End\n");
#endif
  return ret;
}


// #################################################################
// 圧縮データ展開１要素（Load済みのデータから展開）
PFC::E_PFC_ERRORCODE
CPfcRestrationRegionPod:: ExpandData(
              double*   v,          // [out] 展開後の出力領域 （１要素）
              const int stepID,     // [in]  ステップID  (0起点）
              const int index       // [in]  領域先頭からの要素の１次元Index (0起点）
           )
{
  PFC::E_PFC_ERRORCODE ret;
#if 0
  DEBUG_PRINT("---- CPfcRestrationRegionPod:: ExpandData()  Start\n");
  DEBUG_PRINT("      stepID=%d  index=%d\n",stepID,index);
#endif
  int max_layer = CPfcFunction::GetPodMaxLayer( m_numStep );
  int num_base  = int_pow(2,max_layer);
  int mlayer    = max_layer - m_numCalculatedLayer;
  int rnum = 1;
  for(int i=0; i<=mlayer; i++ ) {
    rnum = rnum*2;
  }
  
  // rowNum : 検索する経路の数
  int rowNum  = num_base/rnum;

  // 経路毎の情報
  double* coef       = new double[rowNum];
  int*    base_index = new int[rowNum];
  int*    row_st     = new int[rowNum];
  int*    row_size   = new int[rowNum];

  // 経路情報取得
  ret = GetExpandRowInfo(
                stepID,         // [in]  ステップID
                rowNum,         // [in]  経路の数
                coef,           // [out] 経路毎の係数
                base_index,     // [out] 経路毎の基底のインデックス
                row_st,         // [out] 経路毎の開始位置  (0起点）
                row_size        // [out] 経路毎のサイズ
            );
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("Error GetExpandRowInfo()\n");
    delete[] coef;
    delete[] base_index;
    delete[] row_st;
    delete[] row_size;
    return ret;
  }

  // 成分毎の情報
  int numComp = m_numComponent;
  int* ipos_comp    = new int[numComp];
  int* irow_no_comp = new int[numComp];
  int* ioffset_comp = new int[numComp];
  
  // データpositionの位置決定
  // 配列形状 E_PFC_IJKN
  if( m_arrayShape == PFC::E_PFC_IJKN ) 
  {
    int distance = m_numSize;
    ipos_comp[0] = index;
    for(int i=1; i<numComp; i++)  {
      ipos_comp[i] = ipos_comp[i-1] + distance;
    }
  }
  // 配列形状 E_PFC_NIJK
  else
  {
    int distance = 1;
    ipos_comp[0] = numComp*index;
    for(int i=1; i<numComp; i++)  {
      ipos_comp[i] = ipos_comp[i-1] + distance;
    }
  }
  
  // 成分毎の経路およびオフセット位置の決定
  for(int i=0; i<numComp; i++)  {
    irow_no_comp[i] = -1;
    for(int j = 0; j < rowNum; j++) {
      if( ipos_comp[i]>=row_st[j] && ipos_comp[i]<=(row_st[j]+row_size[j]-1) ) {
        irow_no_comp[i] = j;
        ioffset_comp[i] = ipos_comp[i] - row_st[j];
        break;
      }
    }
    if( irow_no_comp[i] == -1 ) {
      PFC_PRINT("row no error\n");
      delete[] coef;
      delete[] base_index;
      delete[] row_st;
      delete[] row_size;
      delete[] ipos_comp;
      delete[] irow_no_comp;
      delete[] ioffset_comp;
      return PFC::E_PFC_ERROR;
    }
  }

  //------------------------------------------//
  //        出力域に設定                      //
  //------------------------------------------//
  for(int i=0; i<numComp; i++)  {
    int rowNo    = irow_no_comp[i];
    int b_index  = base_index[rowNo];
    int b_offset = ioffset_comp[i];
    
    v[i] = coef[rowNo]*m_pIndexBase[b_index][b_offset];
  }

#if 0
  DEBUG_PRINT("  (row info)\n");
  for(int i=0; i<rowNum; i++)  {
    DEBUG_PRINT("     coef      [%d] = %lf\n",i,coef[i]);
    DEBUG_PRINT("     base_index[%d] = %d\n",i,base_index[i]);
    DEBUG_PRINT("     row_st    [%d] = %d\n",i,row_st[i]);
    DEBUG_PRINT("     row_size  [%d] = %d\n",i,row_size[i]);
  }
  DEBUG_PRINT("  (component info)\n");
  for(int i=0; i<numComp; i++)  {
    DEBUG_PRINT("     ipos_comp   [%d] = %d\n",i,ipos_comp[i]);
    DEBUG_PRINT("     irow_no_comp[%d] = %d\n",i,irow_no_comp[i]);
    DEBUG_PRINT("     ioffset_comp[%d] = %d\n",i,ioffset_comp[i]);
  }
#endif

  delete[] coef;
  delete[] base_index;
  delete[] row_st;
  delete[] row_size;
  delete[] ipos_comp;
  delete[] irow_no_comp;
  delete[] ioffset_comp;

#if 0
  DEBUG_PRINT("---- CPfcRestrationRegionPod:: ExpandData()  End\n");
#endif

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 圧縮データ展開１要素（ファイルから展開）
PFC::E_PFC_ERRORCODE
CPfcRestrationRegionPod:: ExpandDataOnFile(
              double*  v,           // [out] 展開後の出力領域 （１要素）
              const int stepID,     // [in]  ステップID  (0起点）
              const int index       // [in]  領域先頭からの要素の１次元Index
           )
{
  PFC::E_PFC_ERRORCODE ret;
//#if DEBUG_PFC
#if 0
  DEBUG_PRINT("---- CPfcRestrationRegionPod:: ExpandDataOnFile()  Start\n");
  DEBUG_PRINT("      stepID=%d  index=%d\n",stepID,index);
  fflush(stdout);
#endif

  // 基底ファイルヘッダ部読み込み
  //   m_numCalculatedLayerの情報が必要なので先に読む
  ifstream  ifs;
  int       total_header_bsize;
  int       total_base_size;
  bool      endian_chg;
  int       numStep_wk, numCalculatedLayer_wk;

    // file open
  ret = CPfcPodFile::OpenBaseFile(
                        m_dirPath,  // (in)  フィールドデータのディレクトリ
                        m_prefix,   // (in)  属性名
                        m_regionID, // (in)  領域ID
                        m_bSingle,  // (in) 単一領域フラグ
                        ifs         // (out) 入力ファイルストリーム
                    );
  if ( ret != PFC::E_PFC_SUCCESS ) { 
    PFC_PRINT( "### ERROR ### OpenBaseFile()\n" );
    return PFC::E_PFC_ERROR;
  }
    // read header 
  ret = CPfcPodFile::ReadBaseFileHeader(
                        ifs,                    // (in) 入力ファイルストリーム
                        numStep_wk,             // (out) タイムステップ数
                        m_numParallel,          // (out) 圧縮時並列数
                        m_numCalculatedLayer,   // (out) 圧縮時計算レイヤー数
                        m_pBaseSizes,           // (out) 各ランク内の要素数
                        total_header_bsize,     // (out) ヘッダ部のトータルサイズ(byte)
                        total_base_size,        // (out) 基底データのトータルサイズ
                        endian_chg              // (out) エンディアン変換フラグ
            );
  if ( ret != PFC::E_PFC_SUCCESS ) { 
    PFC_PRINT( "### ERROR ### ReadBaseFile()\n" );
    return PFC::E_PFC_ERROR;
  }
#if 0
  DEBUG_PRINT("  -- CPfcPodFile::ReadBaseFileHeader()\n");
  DEBUG_PRINT("       numStep_wk=%d\n",numStep_wk);
  DEBUG_PRINT("       m_numParallel=%d\n",m_numParallel);
  DEBUG_PRINT("       m_numCalculatedLayer=%d\n",m_numCalculatedLayer);
  for(int i=0; i<m_numParallel; i++) {
    DEBUG_PRINT("       m_pBaseSizes[%d]=%d\n",i,m_pBaseSizes[i]);
  }
  DEBUG_PRINT("       total_header_bsize=%d\n",total_header_bsize);
  DEBUG_PRINT("       total_base_size=%d\n",total_base_size);
  DEBUG_PRINT("       endian_chg=%d\n",endian_chg);
  fflush(stdout);
#endif

  // タイムステップ数整合性チェック
  if( numStep_wk != m_numStep ) {
    PFC_PRINT(" numStep error  numStep in PODbase = %d  numStep in Init()=%d\n",
               numStep_wk, m_numStep);
    return PFC::E_PFC_ERROR;
  }

  // 係数ファイル読み込み
  //     係数ファイルは全て読み込む
  ret = CPfcPodFile::ReadCoefFile(
                m_dirPath,  // (in)  フィールドデータのディレクトリ
                m_prefix,   // (in)  属性名
                m_regionID, // (in)  領域ID
                m_bSingle,  // (in) 単一領域フラグ
                numStep_wk,             // (out) タイムステップ数
                                        //          == Layer0の係数出力数
                m_numCoef,              // (out) 係数出力数（Layer1以降）
                numCalculatedLayer_wk,  // (out) 圧縮時計算レイヤー数
                m_pCoefData,            // (out) 係数データ
                                        //         [numCalculatedLayer][numStep];
                                        //          ポインタの参照型であることに注意
                                        //          使用後, delete [] pCoefData
                m_pIndexCoef            // (out) 各レイヤーの係数データへのポインタ
                                        //          [m_numCoef]
                                        //          ポインタの参照型であることに注意
                                        //          使用後, delete [] pIndexCoef 必要

            );

  if ( ret != PFC::E_PFC_SUCCESS ) { 
    PFC_PRINT( "### ERROR ### ReadCoefFile()\n" );
    return PFC::E_PFC_ERROR;
  }


  int max_layer = CPfcFunction::GetPodMaxLayer( m_numStep );
  int num_base  = int_pow(2,max_layer);
  int mlayer    = max_layer - m_numCalculatedLayer;
  int rnum = 1;
  for(int i=0; i<=mlayer; i++ ) {
    rnum = rnum*2;
  }
  
  // rowNum : 検索する経路の数
  int rowNum  = num_base/rnum;
#if 0
  DEBUG_PRINT("       m_numStep           =%d\n",m_numStep);
  DEBUG_PRINT("       max_layer           =%d\n",max_layer);
  DEBUG_PRINT("       m_numCalculatedLayer=%d\n",m_numCalculatedLayer);
  DEBUG_PRINT("       num_base            =%d\n",num_base);
  DEBUG_PRINT("       mlayer              =%d\n",mlayer);
  DEBUG_PRINT("       rowNum              =%d\n",rowNum);
  fflush(stdout);
#endif

  // 経路毎の情報
  double* coef       = new double[rowNum];
  int*    base_index = new int[rowNum];
  int*    row_st     = new int[rowNum];
  int*    row_size   = new int[rowNum];

  // 経路情報取得
  ret = GetExpandRowInfo(
                stepID,         // [in]  ステップID
                rowNum,         // [in]  経路の数
                coef,           // [out] 経路毎の係数
                base_index,     // [out] 経路毎の基底のインデックス
                row_st,         // [out] 経路毎の開始位置  (0起点）
                row_size        // [out] 経路毎のサイズ
            );
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("Error GetExpandRowInfo()\n");
    delete[] coef;
    delete[] base_index;
    delete[] row_st;
    delete[] row_size;
    return PFC::E_PFC_ERROR;
  }

  // 成分毎の情報
  int numComp = m_numComponent;
  int* ipos_comp    = new int[numComp];  // 元データの位置
  int* irow_no_comp = new int[numComp];  // 成分毎の経路No
  int* ioffset_comp = new int[numComp];  // 成分毎のオフセット
  int* ipos_base    = new int[numComp];  // 基底データ上の位置
  
  // データpositionの位置決定
  // 配列形状 E_PFC_IJKN
  if( m_arrayShape == PFC::E_PFC_IJKN ) 
  {
    int distance = m_numSize;
    ipos_comp[0] = index;
    for(int i=1; i<numComp; i++)  {
      ipos_comp[i] = ipos_comp[i-1] + distance;
    }
  }
  // 配列形状 E_PFC_NIJK
  else
  {
    int distance = 1;
    ipos_comp[0] = numComp*index;
    for(int i=1; i<numComp; i++)  {
      ipos_comp[i] = ipos_comp[i-1] + distance;
    }
  }
  
  // 成分毎の経路およびオフセット位置の決定
  for(int i=0; i<numComp; i++)  {
    irow_no_comp[i] = -1;
    for(int j = 0; j < rowNum; j++) {
      if( ipos_comp[i]>=row_st[j] && ipos_comp[i]<=(row_st[j]+row_size[j]-1) ) {
        irow_no_comp[i] = j;
        ioffset_comp[i] = ipos_comp[i] - row_st[j];
        break;
      }
    }
    if( irow_no_comp[i] == -1 ) {
      PFC_PRINT("row no error\n");
      delete[] coef;
      delete[] base_index;
      delete[] row_st;
      delete[] row_size;
      delete[] ipos_comp;
      delete[] irow_no_comp;
      delete[] ioffset_comp;
      delete[] ipos_base;
      return PFC::E_PFC_ERROR;
    }
  }
  
  // 基底データ上の位置決定
  for(int i=0; i<numComp; i++)  {
    ipos_base[i] = 0;
    int rowNo = irow_no_comp[i];
    int b_index  = base_index[rowNo];
    
    //for(int j=1; j<b_index; j++ ) {
    //  ipos_base[i] += m_pBaseSizes[j-1];
    for(int j=0; j<b_index; j++ ) {
      ipos_base[i] += m_pBaseSizes[j];
    }
    ipos_base[i] += ioffset_comp[i];
  }

  //------------------------------------------//
  // ファイルから読み込んで設定               //
  //------------------------------------------//
  for(int i=0; i<numComp; i++)  {
    double val_b;

    ret = CPfcPodFile::ReadBaseFile1Elm(
                ifs,                // (in)  入力ファイルストリーム
                endian_chg,         // (in)  エンディアン変換フラグ
                total_header_bsize, // (in)  ヘッダ部のトータルサイズ(byte)
                ipos_base[i],       // (in)  基底データ部の読み込みインデックス
                val_b               // (out) 基底データ(１成分)
            );
    if( ret !=PFC::E_PFC_SUCCESS ) {
      PFC_PRINT("Error ReadBaseFile1Elm()\n");
      delete[] coef;
      delete[] base_index;
      delete[] row_st;
      delete[] row_size;
      delete[] ipos_comp;
      delete[] irow_no_comp;
      delete[] ioffset_comp;
      delete[] ipos_base;
      return PFC::E_PFC_ERROR;
    }

    int rowNo    = irow_no_comp[i];
    v[i] = coef[rowNo]*val_b;   // 要素データ設定
  }

  //------------------------------------------//
  //  終了化処理                              //
  //------------------------------------------//
  // file close
  CPfcPodFile::CloseBaseFile( ifs );

  // free
  if( m_pBaseSizes != NULL ) {
    delete [] m_pBaseSizes; m_pBaseSizes = NULL;
  }
  if( m_pIndexCoef != NULL ) {
    delete [] m_pIndexCoef; m_pIndexCoef = NULL;
  }
  if( m_pCoefData != NULL ) {
    delete [] m_pCoefData; m_pCoefData = NULL;
  }

  delete[] coef;
  delete[] base_index;
  delete[] row_st;
  delete[] row_size;
  delete[] ipos_comp;
  delete[] irow_no_comp;
  delete[] ioffset_comp;
  delete[] ipos_base;

#if 0
  DEBUG_PRINT("---- CPfcRestrationRegionPod:: ExpandDataOnFile()  End\n");
#endif

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 圧縮データ展開用経路情報取得
//      係数ファイルは読み込み済みであること
//      基底ファイルのヘッダは読み込み済みであること
PFC::E_PFC_ERRORCODE
CPfcRestrationRegionPod::GetExpandRowInfo(
            const int stepID,       // [in]  ステップID
            const int rowNum,       // [in]  経路の数
            double*   coef,         // [out] 経路毎の係数  size:[row_num]
            int*      base_index,   // [out] 経路毎の基底のインデックス size:[row_num]
                                    //         pod baseファイルを出力したランク番号
            int*      row_st,       // [out] 経路毎の開始位置  (0起点）size:[row_num]
            int*      row_size      // [out] 経路毎のサイズ size:[row_num]
                                    //         == base_index毎のサイズ
        )
{
  int* stepID_layer = new int[m_numCalculatedLayer];
#if 0
  DEBUG_PRINT("   GetExpandRowInfo() Start  stepID=%d rowNum=%d\n",
                           stepID,rowNum );
  fflush(stdout);
#endif

  // 係数ツリー生成
  CPfcCoeffTree* mytree = new CPfcCoeffTree(
                                    m_numStep,
                                    m_numCalculatedLayer
                                );
  mytree->construct_tree(stepID);

#if 0
  DEBUG_PRINT("   GetExpandRowInfo() Loop rowNum start\n");
  fflush(stdout);
#endif
  // 経路の数ループ
  for(int i = 0; i < rowNum; i++){

    //------------------------------------------//
    //              coefficient                 //             
    //------------------------------------------//
    mytree->deep_transerval(stepID_layer,i);

    coef[i] = 1.0;
    for(int k = 0; k < m_numCalculatedLayer; k++) {
      int stepID_wk = stepID_layer[k];
#if 0
      DEBUG_PRINT("GetExpandRowInfo() i=%d  stepID_layer[%d]=%d\n",
                         i, k, stepID_layer[k] );
      fflush(stdout);
#endif
      coef[i] = coef[i]*m_pIndexCoef[k][stepID_wk];
    }

    //------------------------------------------//
    //              pod base                    //
    //------------------------------------------//
    int stepID_base = stepID_layer[m_numCalculatedLayer-1];
    
    // base index: pod baseファイルを出力したランク番号
    base_index[i] = CPfcFunction::GetPodBaseIndex(
                           m_numStep, m_numParallel, 
                           m_numCalculatedLayer, stepID_base
                         );
    if( base_index[i] < 0 || base_index[i] >= m_numParallel ) {
      PFC_PRINT("base_index not found:  base_index[%d]=%d\n",i,base_index[i]);
      fflush(stdout);
      delete[] stepID_layer;
      return PFC::E_PFC_ERROR;
    }
    
    row_size[i] = m_pBaseSizes[base_index[i]];  // 経路毎のbase size->row_size

  }
#if 0
  DEBUG_PRINT("   GetExpandRowInfo() Loop rowNum end\n");
  fflush(stdout);
#endif
  
  row_st[0] = 0;
  for(int i = 1; i < rowNum; i++){
    row_st[i] = row_st[i-1] + row_size[i-1];
  }

  delete mytree;
  delete[] stepID_layer;

  return PFC::E_PFC_SUCCESS;
}
