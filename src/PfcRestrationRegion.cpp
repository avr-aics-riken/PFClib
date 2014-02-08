/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcRestrationRegion.cpp  分割領域 展開クラス
 * @brief  CPfcRestrationRegion Class
 * @author aics
 */

#include "PfcRestrationRegion.h"
#include "PfcPodFile.h"
#include "PfcFunction.h"


// #################################################################
// コンストラクタ
CPfcRestrationRegion::CPfcRestrationRegion( void )
{
  m_pFileInfo     = NULL;
  m_pCompressInfo = NULL;
  m_pDomain       = NULL;
  m_pRegion       = NULL;

  m_compressForm  = PFC::E_PFC_COMPRESS_FMT_UNKNOWN;
  pPod            = NULL;
  m_bLoadCompressData = false;  // 圧縮データのメモリロードフラグ
}


// #################################################################
// デストラクタ
CPfcRestrationRegion::~CPfcRestrationRegion()
{
  DeleteCompressDataOnMem();
  
  if( pPod != NULL ) delete pPod;
}

// #################################################################
// 分割領域 展開クラス 初期化
PFC::E_PFC_ERRORCODE
CPfcRestrationRegion::Init(
           CPfcFileInfo*     pFileInfo,       // [in] File情報（属性etc.）
           CPfcCompressInfo* pCompressInfo,   // [in] 圧縮情報
           vector<int>*      pvTimeStepList,  // [in] タイムステップリスト(vecterのpointer)
           CPfcDomain*       pDomain,         // [in] ドメイン情報
           CPfcRegion*       pRegion          // [in] 分割領域情報（１領域）
         )
{
  PFC::E_PFC_ERRORCODE ret = PFC::E_PFC_SUCCESS;
  m_pFileInfo       = pFileInfo;
  m_pCompressInfo   = pCompressInfo;
  m_pvTimeStepList  = pvTimeStepList;
  m_pDomain         = pDomain;
  m_pRegion         = pRegion;

  if( pCompressInfo->m_compressFormat == "pod" )   {
    m_compressForm = PFC::E_PFC_COMPRESS_FMT_POD;
  } else {
    PFC_PRINT(" compress format error  compressFormat=%s\n",
                     pCompressInfo->m_compressFormat.c_str() );
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }
  
  if( m_compressForm == PFC::E_PFC_COMPRESS_FMT_POD ) 
  {
    pPod = new CPfcRestrationRegionPod();
    
    int numSize = pRegion->m_voxelSize[0]*pRegion->m_voxelSize[1]*pRegion->m_voxelSize[2];
    int numRegion = pDomain->m_globalDivision[0] * pDomain->m_globalDivision[1] * pDomain->m_globalDivision[2];
    bool bSingle;
    if( numRegion == 1 ) {
      bSingle = true;
    } else {
      bSingle = false;
    } 
    
    ret = pPod->Init( pFileInfo->m_dirPath,       // (in)  フィールドデータのディレクトリ
                pFileInfo->m_prefix,        // (in)  属性名
                m_pRegion->m_regionID,      // (in)  領域ID
                numSize,                    // (in)  要素数
#ifndef PFC_POD_USE_ORIG_ARRAYSHAPE
                PFC::E_PFC_IJKN,            // (in)  配列形状 (POD圧縮内部データ：E_PFC_IJKN 固定）
#else
                pFileInfo->m_arrayShape,    // (in)  配列形状    E_PFC_IJKN / E_PFC_NIJK
#endif
                pFileInfo->m_numComponent,  // (in)  成分数
                pvTimeStepList->size(),     // (in)  タイムステップ数
                bSingle                     // (in)  単一領域フラグ
             );
    
    if( ret != PFC::E_PFC_SUCCESS ) {
      PFC_PRINT(" ERROR CPfcRestrationRegionPod::Init()\n" );\
    }
  }
  
  return ret;
}


// #################################################################
// 圧縮データのメモリロード（分割領域の全データロード）
PFC::E_PFC_ERRORCODE
CPfcRestrationRegion::LoadCompressDataOnMem( void )
{
#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcRestrationRegion::LoadCompressDataOnMem() Start\n");
  fflush(stdout);
#endif
  PFC::E_PFC_ERRORCODE ret;
  
  if( m_compressForm == PFC::E_PFC_COMPRESS_FMT_POD )
  {
    ret = pPod->LoadCompressDataOnMem();

    if ( ret != PFC::E_PFC_SUCCESS ) { 
      PFC_PRINT( "### ERROR ### ReadBaseFile()\n" );
      return PFC::E_PFC_ERROR;
    }
  }
  else
  {
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }

  // 圧縮データのメモリロードフラグ
  m_bLoadCompressData = true;

#ifdef DEBUG_PFC
  DEBUG_PRINT("---- CPfcRestrationRegion::LoadCompressDataOnMem() End\n");
  fflush(stdout);
#endif
  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// ロードした圧縮データ削除
void
CPfcRestrationRegion::DeleteCompressDataOnMem( void )
{
  if( m_compressForm == PFC::E_PFC_COMPRESS_FMT_POD )
  {
    pPod->DeleteCompressDataOnMem();
  }

  // 圧縮データのメモリロードフラグ
  m_bLoadCompressData = false;
}


// #################################################################
// データ読み込み（範囲指定）
//    LoadCompressDataOnMem()が実行されている場合、メモリから展開する
//    対象計算領域と自身が管理している領域が一致していない場合、AND領域のみが設定される
PFC::E_PFC_ERRORCODE
CPfcRestrationRegion::ReadDataInRange (
              double*  v,           // [out] 展開後の出力領域 
                                    //       メモリは確保済みであること
                                    //       領域サイズ：head-tail間のボクセルサイズ*成分数
                                    //       設定領域：out_offset_stからout_size分
              const int step,       // [in]  ステップ番号
              const int head[3],    // [in]  対象計算領域の開始位置
                                    //          (自身の領域のサイズではない）
              const int tail[3],    // [in]  対象計算領域の終了位置
                                    //          (自身の領域のサイズではない）
              int out_offset_st[3], // [out] 出力領域のheadからのオフセット
              int out_size  [3]     // [out] 出力領域のサイズ
           )
{
  PFC::E_PFC_ERRORCODE ret;
  int     nsize_region = m_pFileInfo->m_numComponent
                         * m_pRegion->m_voxelSize[0]*m_pRegion->m_voxelSize[1]*m_pRegion->m_voxelSize[2];
  double* pRegionData = new double[nsize_region];
  
  int lap_offset1[3],lap_offset2[3],lap_size[3];
  int nsize_to [3];
  
  // stepID取得
  int stepID = -1;
  for(int i=0; i<(*m_pvTimeStepList).size(); i++ ) {
    if( step == (*m_pvTimeStepList)[i] ) {
      stepID = i;
      break;
    }
  }
  if( stepID == -1 ) {
    PFC_PRINT("Step not exist step=%d\n",step);
    return PFC::E_PFC_ERROR;
  }
  
  // 領域データ取得
  if( m_compressForm == PFC::E_PFC_COMPRESS_FMT_POD )
  {
    //ret = pPod->ReadFieldData( pRegionData, stepID );
    ret = pPod->ReadData( m_pFileInfo->m_arrayShape,
                          pRegionData, stepID );
    
    if( ret != PFC::E_PFC_SUCCESS ) {
      PFC_PRINT("error at ReadFieldData()\n");
      return PFC::E_PFC_ERROR;
    }
  }
  else
  {
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }
  
  // Min/Max(Head/Tail)領域 のＡＮＤ（重なり）取得
  ret = CPfcFunction::AndMinMax( 
                            head,                   // [in]  対象計算領域の開始位置
                            tail,                   // [in]  対象計算領域の終了位置
                            m_pRegion->m_headIndex, // [in]  自身の領域の開始位置
                            m_pRegion->m_tailIndex, // [in]  自身の領域の終了位置
                            lap_offset1,            // [out] 対象計算領域の重なり区間までのオフセット
                            lap_offset2,            // [out] 自身の領域の重なり区間までのオフセット
                            lap_size                // [out] ＡＮＤ（重なり）領域のサイズ
                        );

  if( ret != PFC::E_PFC_SUCCESS ) {
    delete[] pRegionData;
    PFC_PRINT("Regions not overlap\n");
    return PFC::E_PFC_ERROR;
  }
  
  // データコピー（範囲指定）
  nsize_to[0] = tail[0] - head[0] + 1;
  nsize_to[1] = tail[1] - head[1] + 1;
  nsize_to[2] = tail[2] - head[2] + 1;

  CPfcFunction::CopyData (
                  v,                        // [out] コピー先配列のアドレス
                  nsize_to,                 // [in]  コピー先配列のサイズ
                  lap_offset1,              // [in]  コピー先の配列先頭からのオフセット(0～)
                  pRegionData,              // [in]  コピー元配列のアドレス
                  m_pRegion->m_voxelSize,   // [in]  コピー元配列のサイズ
                  lap_offset2,              // [in]  コピー元の配列先頭からのオフセット(0～)
                  lap_size,                 // [in]  コピーサイズ
                  m_pFileInfo->m_numComponent,  // [in]  成分数
                  m_pFileInfo->m_arrayShape     // [in]  配列形状 E_PFC_IJKN/E_PFC_NIJK
           );
  
  // 出力領域情報設定
  out_offset_st[0]=lap_offset1[0]; out_offset_st[1]=lap_offset1[1]; out_offset_st[2]=lap_offset1[2];
  out_size[0] = lap_size[0]; out_size[1] = lap_size[1]; out_size[2] = lap_size[2];
  
  
  delete[] pRegionData;

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// データ読み込み（位置指定）
//    LoadCompressDataOnMem()が実行されている場合、メモリから展開する
PFC::E_PFC_ERRORCODE
CPfcRestrationRegion::ReadData (
              double*  v,       // [out] 展開後の出力領域（１要素）
              const int step,   // [in]  ステップ番号
              const int ix,     // [in]  x position headの開始は1からのため、それに合わせる
              const int iy,     // [in]  y position headの開始は1からのため、それに合わせる
              const int iz      // [in]  z position headの開始は1からのため、それに合わせる
           )
{
  PFC::E_PFC_ERRORCODE ret;
  int ix0, iy0, iz0, index;
  
  // ローカルオフセットに変更
  ix0 = ix - m_pRegion->m_headIndex[0];
  iy0 = iy - m_pRegion->m_headIndex[1];
  iz0 = iz - m_pRegion->m_headIndex[2];
  
  int* sz = m_pRegion->m_voxelSize;
  index = sz[0]*sz[1]*iz0 + sz[0]*iy0 + ix0;
  

  // ステップ番号->stepID(SerNo)取得
  int stepID = -1;
  for(int i=0; i<m_pvTimeStepList->size(); i++ ) {
    if( step == (*m_pvTimeStepList)[i] ) {
      stepID = i;
      break;
    }
  }
  if( stepID == -1 ) {
    PFC_PRINT("Step not exist step=%d\n",step);
    return PFC::E_PFC_ERROR;
  }

  if( m_compressForm == PFC::E_PFC_COMPRESS_FMT_POD )
  {
    // 圧縮データ ロード済み
    if( m_bLoadCompressData ) 
    {
      ret = pPod->ExpandData( v,
                            stepID, index );
      if( ret != PFC::E_PFC_SUCCESS ) {
        PFC_PRINT("Error ExpandDataOnMem()\n");
        return PFC::E_PFC_ERROR;
      }
    }
    // 圧縮データ ロードなし
    else
    {
      ret = pPod->ExpandDataOnFile( v,
                            stepID, index );
      if( ret != PFC::E_PFC_SUCCESS ) {
        PFC_PRINT("Error ExpandDataOnFile()\n");
        return PFC::E_PFC_ERROR;
      }
    }
  }
  else
  {
    return PFC::E_PFC_ERROR_PFC_COMPRESSFORMAT;
  }

  return PFC::E_PFC_SUCCESS;
}

