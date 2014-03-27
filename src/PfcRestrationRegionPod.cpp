/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcRestrationRegionPod.cpp  分割領域 展開クラス（１次元で操作する）
 * @brief  CPfcRestrationRegionPod Class
 * @author aics
 */

#include "PfcRestrationRegionPod.h"
#include "PfcPodFile.h"
#include "PfcFunction.h"


// #################################################################
// コンストラクタ
CPfcRestrationRegionPod::CPfcRestrationRegionPod( void )
{
  m_bLoadCompressData = false; // 圧縮データのメモリロードフラグ
  
  m_dirPath       = "";
  m_prefix        = "";
  m_regionID      = 0;
  m_numSize       = 0;
  m_arrayShape    = PFC::E_PFC_ARRAYSHAPE_UNKNOWN;
  m_numComponent  = 0;
  m_numStep       = 0;
  m_bSingle       = false;

  m_pIndexBase  = NULL;
  m_pBaseData   = NULL;
  m_pBaseSizes  = NULL;
  m_pIndexCoef  = NULL;
  m_pCoefData   = NULL;
}


// #################################################################
// デストラクタ
CPfcRestrationRegionPod::~CPfcRestrationRegionPod()
{
  DeleteCompressDataOnMem();
}

// #################################################################
// 分割領域 展開クラス 初期化
PFC::E_PFC_ERRORCODE
CPfcRestrationRegionPod::Init(
           const std::string&      dirPath,        // [in] フィールドデータ出力ディレクトリパス
           const std::string&      prefix,         // [in] 属性名
           const int               regionID,       // [in] 領域ID
           const int               numSize,        // [in] 要素数 (voxelサイズ）
           PFC::E_PFC_ARRAYSHAPE   arrayShape,     // [in] 配列形状    E_PFC_IJKN / E_PFC_NIJK
           int                     numComponent,   // [in] 成分数
           int                     numStep,        // [in] タイムステップ数
           bool                    bSingle         // [in] 単一領域フラグ
         )
{
  m_dirPath      = dirPath;        // フィールドデータ出力ディレクトリパス
  m_prefix       = prefix;         // 属性名
  m_regionID     = regionID;       // 領域ID
  m_numSize      = numSize;        // 要素数
  m_arrayShape   = arrayShape;     // 配列形状    E_PFC_IJKN / E_PFC_NIJK
  m_numComponent = numComponent;   // 成分数
  m_numStep      = numStep;        // タイムステップ数
  m_bSingle      = bSingle;        // 単一領域フラグ


  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 圧縮データのメモリロード（分割領域の全データロード）
PFC::E_PFC_ERRORCODE
CPfcRestrationRegionPod::LoadCompressDataOnMem( void )
{
  PFC::E_PFC_ERRORCODE ret;
  int numStep_wk, numCalculatedLayer_wk;

  //------------------------------------------
  // 基底データ読み込み
  //------------------------------------------
  ret = CPfcPodFile::ReadBaseFile(
                        m_dirPath,              // (in)  フィールドデータのディレクトリ
                        m_prefix,               // (in)  属性名
                        m_regionID,             // (in)  領域ID
                        m_bSingle,              // (in) 単一領域フラグ
                        numStep_wk,             // (out) タイムステップ数
                        m_numParallel,          // (out) 圧縮時並列数
                        m_numCalculatedLayer,   // (out) 圧縮時計算レイヤー数
                        m_pBaseSizes,           // (out) 各ランク内の要素数
                                                //          ポインタの参照型であることに注意
                                                //          pSizes[m_numParallel]
                                                //          使用後, delete [] pSizes 必要
                        m_pBaseData,            // (out) 基底データ
                                                //          ポインタの参照型であることに注意
                                                //          使用後, delete [] pPod_base 必要
                        m_pIndexBase            // (out) 各ランク内の基底データへのポインタ
                                                //          ポインタの参照型であることに注意
                                                //          使用後, delete [] pIndex 必要
            );
  if ( ret != PFC::E_PFC_SUCCESS ) { 
    PFC_PRINT( "### ERROR ### ReadBaseFile()\n" );
    return PFC::E_PFC_ERROR;
  }
  
  // タイムステップ数整合性チェック
  if( numStep_wk != m_numStep ) {
    PFC_PRINT(" numStep error  numStep in PODbase = %d  numStep in Init()=%d\n",
               numStep_wk, m_numStep);
    return PFC::E_PFC_ERROR;
  }

  //------------------------------------------
  // 係数データ読み込み
  //------------------------------------------

  
  ret = CPfcPodFile::ReadCoefFile(
                m_dirPath,              // (in)  フィールドデータのディレクトリ
                m_prefix,               // (in)  属性名
                m_regionID,             // (in)  領域ID
                m_bSingle,              // (in) 単一領域フラグ
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
  // タイムステップ数整合性チェック
  if( numStep_wk != m_numStep ) {
    PFC_PRINT(" numStep error  numStep in PODcoef = %d  numStep in Init()=%d\n",
               numStep_wk, m_numStep);
    return PFC::E_PFC_ERROR;
  }
  // 計算レイヤー数整合性チェック
  if( numCalculatedLayer_wk != m_numCalculatedLayer ) {
    PFC_PRINT(" numCalculatedLayer error  numCalculatedLayer in PODcoef = %d  numCalculatedLayer in PODbase=%d\n",
               numCalculatedLayer_wk, m_numCalculatedLayer );
    return PFC::E_PFC_ERROR;
  }

  // 圧縮データのメモリロードフラグ
  m_bLoadCompressData = true;

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// ロードした圧縮データ削除
void
CPfcRestrationRegionPod::DeleteCompressDataOnMem( void )
{

  if( m_pIndexBase != NULL ) {
    delete [] m_pIndexBase; m_pIndexBase = NULL;
  }
  if( m_pBaseData  != NULL ) {
    delete [] m_pBaseData; m_pBaseData = NULL;
  }
  if( m_pBaseSizes != NULL ) {
    delete [] m_pBaseSizes; m_pBaseSizes = NULL;
  }
  if( m_pIndexCoef != NULL ) {
    delete [] m_pIndexCoef; m_pIndexCoef = NULL;
  }
  if( m_pCoefData  != NULL ) {
    delete [] m_pCoefData; m_pCoefData = NULL;
  }

  // 圧縮データのメモリロードフラグ
  m_bLoadCompressData = false;
}


// #################################################################
// フィールドデータ読み込み（領域内全情報） 配列形状指定
//    LoadCompressDataOnMem()が実行されている場合、メモリから展開する
PFC::E_PFC_ERRORCODE
CPfcRestrationRegionPod::ReadData (
         PFC::E_PFC_ARRAYSHAPE  arrayShape,  // [in] 配列形状    E_PFC_IJKN / E_PFC_NIJK
         double*                v,           // [out] 展開後の出力領域 
                                             //       メモリは確保済みであること
                                             //       region内のhead-tail間のボクセルサイズ*成分数
         const int stepID                    // [in]  ステップID (起点0)
       )
{
  PFC::E_PFC_ERRORCODE ret=PFC::E_PFC_SUCCESS;

  // POD 圧縮内部データの配列形状 IJKN と一致
  if( arrayShape == m_arrayShape ) 
  {
#ifdef USE_PMLIB
    PfcPerfMon::Start(t251_r);
#endif
    ret = ReadFieldData( v, stepID );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t251_r);
#endif
  }
  // 読み出し配列形状 NIJK
  else
  {
    double* dwk_ijkn = new double[m_numComponent*m_numSize];
    PFC::E_PFC_ERRORCODE ret;

#ifdef USE_PMLIB
    PfcPerfMon::Start(t251_r);
#endif
    ret = ReadFieldData( dwk_ijkn, stepID );
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t251_r);
#endif
    if( ret != PFC::E_PFC_SUCCESS ) {
      delete[] dwk_ijkn;
      return ret;
    }

    // IJKN -> NIJK 変換
    for ( int i =0; i<m_numSize; i++ ) {
      for ( int j =0; j<m_numComponent; j++ ) {
        v[m_numComponent*i+j] = dwk_ijkn[m_numSize*j+i];
      }
    }

    delete[] dwk_ijkn;
  }

  return ret;
}



// #################################################################
// フィールドデータ読み込み（領域内全情報）
//    LoadCompressDataOnMem()が実行されている場合、メモリから展開する
PFC::E_PFC_ERRORCODE
CPfcRestrationRegionPod::ReadFieldData (
              double*  v,           // [out] 展開後の出力領域 
                                    //       メモリは確保済みであること
                                    //       region内のhead-tail間のボクセルサイズ*成分数
              const int stepID      // [in]  ステップID (起点0)
           )
{

  PFC::E_PFC_ERRORCODE ret;
  bool bNewLoad = false;

  // 圧縮データのメモリロード（分割領域の全データロード）
  if( !m_bLoadCompressData ) {
#ifdef USE_PMLIB
    PfcPerfMon::Start(t261_r);
#endif
    ret = LoadCompressDataOnMem();
#ifdef USE_PMLIB
    PfcPerfMon::Stop(t261_r);
#endif
    if( ret != PFC::E_PFC_SUCCESS ) {
      PFC_PRINT("Error LoadCompressDataOnMem\n");
      fflush(stdout);
      return ret;
    }
    bNewLoad = true;
  }
  
  // 展開処理
#ifdef USE_PMLIB
  PfcPerfMon::Start(t263_r);
#endif

  ret = ExpandData ( v, stepID );

#ifdef USE_PMLIB
  PfcPerfMon::Stop(t263_r);
#endif
  if( ret != PFC::E_PFC_SUCCESS ) {
    PFC_PRINT("Error Expand\n");
    return ret;
  }

  // 新規に圧縮した場合、圧縮データ 削除
  if( bNewLoad ) {
    DeleteCompressDataOnMem();
  }


  return PFC::E_PFC_SUCCESS;
}

