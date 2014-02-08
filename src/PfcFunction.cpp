/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcFunction.cpp 
 * @brief  CPfcFunction Class
 * @author aics    
 */

#include <math.h>
#include <algorithm>
#include "PfcFunction.h"
using namespace std;


// #################################################################
// 領域範囲(Head/Tail)計算 
//    ex)  95 を10で分割する場合
//          ->  10 10 10 10 10  9  9  9  9  9
PFC::E_PFC_ERRORCODE
CPfcFunction::CalcHeadTail(
                  const int voxel[3], 
                  const int div[3], 
                  const int regionID, 
                  int       head[3],
                  int       tail[3]
                )
{
  int isize_x = voxel[0]/div[0]; int mod_x = voxel[0]%div[0];
  int isize_y = voxel[1]/div[1]; int mod_y = voxel[1]%div[1];
  int isize_z = voxel[2]/div[2]; int mod_z = voxel[2]%div[2];
  
  int id_x = regionID % div[0];
  int id_y = (regionID/div[0]) % div[1];
  int id_z = regionID / (div[0]*div[1]);

  head[0] = isize_x*id_x + min(mod_x,id_x) + 1;
  head[1] = isize_y*id_y + min(mod_y,id_y) + 1;
  head[2] = isize_z*id_z + min(mod_z,id_z) + 1;

  tail[0] = isize_x*(id_x+1) + min(mod_x,id_x+1);
  tail[1] = isize_y*(id_y+1) + min(mod_y,id_y+1);
  tail[2] = isize_z*(id_z+1) + min(mod_z,id_z+1);

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 領域範囲(Head/Tail)計算 （単純ブロック分割）
//    ex)  95 を10で分割する場合
//          ->  10 10 10 10 10 10 10 10 10  5
PFC::E_PFC_ERRORCODE
CPfcFunction::CalcHeadTail_block(
                  const int voxel[3], 
                  const int div[3], 
                  const int regionID, 
                  int       head[3],
                  int       tail[3]
                )
{
  int isize_x = voxel[0]/div[0]; if( (voxel[0]%div[0]) != 0 )  isize_x++;
  int isize_y = voxel[1]/div[1]; if( (voxel[1]%div[1]) != 0 )  isize_y++;
  int isize_z = voxel[2]/div[2]; if( (voxel[2]%div[2]) != 0 )  isize_z++;
  
  int id_x = regionID % div[0];
  int id_y = (regionID/div[0]) % div[1];
  int id_z = regionID / (div[0]*div[1]);

  head[0] = isize_x*id_x + 1;
  head[1] = isize_y*id_y + 1;
  head[2] = isize_z*id_z + 1;

  tail[0] = min( (head[0]+isize_x-1), voxel[0] );
  tail[1] = min( (head[1]+isize_y-1), voxel[1] );
  tail[2] = min( (head[2]+isize_z-1), voxel[2] );

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// データコピー（範囲指定）
void
CPfcFunction::CopyData(
                  double*   to,                 // [out] コピー先配列のアドレス
                  const int nsize_to [3],       // [in]  コピー先配列のサイズ
                  const int offset_to[3],       // [in]  コピー先の配列先頭からのオフセット(0～)
                  const double*   from,         // [in]  コピー元配列のアドレス
                  const int nsize_from [3],     // [in]  コピー元配列のサイズ
                  const int offset_from[3],     // [in]  コピー元の配列先頭からのオフセット(0～)
                  const int nsize_copy [3],     // [in]  コピーサイズ
                  const int num_component,      // [in]  成分数
                  PFC::E_PFC_ARRAYSHAPE array_shape   // [in] 配列形状 E_PFC_IJKN/E_PFC_NIJK
           )
{
   // ALL Copy : Check & Copy
   if( nsize_copy[0]==nsize_to[0] && nsize_copy[0]==nsize_from[0] &&
       nsize_copy[1]==nsize_to[1] && nsize_copy[1]==nsize_from[1] &&
       nsize_copy[2]==nsize_to[2] && nsize_copy[2]==nsize_from[2]     ) {
       
       for(int i=0; i<num_component*nsize_copy[0]*nsize_copy[1]*nsize_copy[2]; i++)
       {
         *(to+i) = *(from+i);
       }
   }
 
   
   // 配列形状 E_PFC_IJKN
   if( array_shape == PFC::E_PFC_IJKN )
   {
     for(int m=0; m<num_component; m++)  {
       int offset_m_to   = nsize_to  [0]*nsize_to  [1]*nsize_to  [2]*m;
       int offset_m_from = nsize_from[0]*nsize_from[1]*nsize_from[2]*m;

       for(int k=0; k<nsize_copy[2]; k++)  {
         int offset_k_to   = nsize_to  [0]*nsize_to  [1]*(offset_to  [2]+k);
         int offset_k_from = nsize_from[0]*nsize_from[1]*(offset_from[2]+k);

         for(int j=0; j<nsize_copy[1]; j++)  {
           int offset_j_to   = nsize_to  [0]*(offset_to  [1]+j);
           int offset_j_from = nsize_from[0]*(offset_from[1]+j);

           for(int i=0; i<nsize_copy[0]; i++)  {
             to[offset_m_to+offset_k_to+offset_j_to+(offset_to[0]+i)]
               = from[offset_m_from+offset_k_from+offset_j_from+(offset_from[0]+i)];
           }
         }
       }
     }
   }
   
   // 配列形状 E_PFC_NIJK
   else
   {
     for(int k=0; k<nsize_copy[2]; k++)  {
         int offset_k_to   = num_component*nsize_to  [0]*nsize_to  [1]*(offset_to  [2]+k);
         int offset_k_from = num_component*nsize_from[0]*nsize_from[1]*(offset_from[2]+k);

         for(int j=0; j<nsize_copy[1]; j++)  {
           int offset_j_to   = num_component*nsize_to  [0]*(offset_to  [1]+j);
           int offset_j_from = num_component*nsize_from[0]*(offset_from[1]+j);

           for(int i=0; i<nsize_copy[0]; i++)  {
             int offset_i_to   = num_component*(offset_to  [0]+i);
             int offset_i_from = num_component*(offset_from[0]+i);
           
             for(int m=0; m<num_component; m++)  {
               to[offset_k_to+offset_j_to+offset_i_to+m]
                 = from[offset_k_from+offset_j_from+offset_i_from+m];
           }
         }
       }
     }
     
   }
}


// #################################################################
// Min/Max(Head/Tail)領域 のＡＮＤ（重なり）取得
PFC::E_PFC_ERRORCODE
CPfcFunction::AndMinMax(
                  const int head1[3],       // [in]  計算領域１の開始位置
                  const int tail1[3],       // [in]  計算領域１の終了位置
                  const int head2[3],       // [in]  計算領域２の開始位置
                  const int tail2[3],       // [in]  計算領域２の終了位置
                  int       lap_offset1[3], // [out] 計算領域１の重なり区間までのオフセット
                  int       lap_offset2[3], // [out] 計算領域２の重なり区間までのオフセット
                  int       lap_size[3]     // [out] ＡＮＤ（重なり）領域のサイズ
           )
{
  int ist1,ist2,ist3;
  int iend1,iend2,iend3;

  // dim1 ------
  if( head1[0]>tail2[0] || tail1[0]<head2[0] ) {
    return PFC::E_PFC_ERROR;
  }
  
  if( head1[0] > head2[0] ) {
    ist1 = head1[0];
  } else {
    ist1 = head2[0];
  }
  
  if( tail1[0] > tail2[0] ) {
    iend1 = tail2[0];
  } else {
    iend1 = tail1[0];
  }

  // dim2 ------
  if( head1[1]>tail2[1] || tail1[1]<head2[1] ) {
    return PFC::E_PFC_ERROR;
  }
  
  if( head1[1] > head2[1] ) {
    ist2 = head1[1];
  } else {
    ist2 = head2[1];
  }
  
  if( tail1[1] > tail2[1] ) {
    iend2 = tail2[1];
  } else {
    iend2 = tail1[1];
  }

  // dim3 ------
  if( head1[2]>tail2[2] || tail1[2]<head2[2] ) {
    return PFC::E_PFC_ERROR;
  }
  
  if( head1[2] > head2[2] ) {
    ist3 = head1[2];
  } else {
    ist3 = head2[2];
  }
  
  if( tail1[2] > tail2[2] ) {
    iend3 = tail2[2];
  } else {
    iend3 = tail1[2];
  }
  
  // set over lap range
  lap_offset1[0] = ist1 - head1[0];
  lap_offset1[1] = ist2 - head1[1];
  lap_offset1[2] = ist3 - head1[2];
  
  lap_offset2[0] = ist1 - head2[0];
  lap_offset2[1] = ist2 - head2[1];
  lap_offset2[2] = ist3 - head2[2];
  
  lap_size[0] = iend1 - ist1 + 1;
  lap_size[1] = iend2 - ist2 + 1;
  lap_size[2] = iend3 - ist3 + 1;

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// Min/Max(Head/Tail)領域 の重なりチェック
PFC::E_PFC_ERRORCODE
CPfcFunction::CheckLapMinMax(
                  const int head1[3],       // [in]  計算領域１の開始位置
                  const int tail1[3],       // [in]  計算領域１の終了位置
                  const int head2[3],       // [in]  計算領域２の開始位置
                  const int tail2[3]        // [in]  計算領域２の終了位置
           )
{
  if( head1[0]>tail2[0] || tail1[0]<head2[0] ) {
    return PFC::E_PFC_ERROR;
  }

  if( head1[1]>tail2[1] || tail1[1]<head2[1] ) {
    return PFC::E_PFC_ERROR;
  }

  if( head1[2]>tail2[2] || tail1[2]<head2[2] ) {
    return PFC::E_PFC_ERROR;
  }

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 点のMin/Max(Head/Tail)領域内チェック
PFC::E_PFC_ERRORCODE
CPfcFunction::CheckPntInMinMax(
                  const int head[3],        // [in]  計算領域の開始位置
                  const int tail[3],        // [in]  計算領域の終了位置
                  const int ix,             // [in]  点のX座標
                  const int iy,             // [in]  点のY座標
                  const int iz              // [in]  点のZ座標
           )
{
  if( ix < head[0] ) return PFC::E_PFC_ERROR;
  if( ix > tail[0] ) return PFC::E_PFC_ERROR;

  if( iy < head[1] ) return PFC::E_PFC_ERROR;
  if( iy > tail[1] ) return PFC::E_PFC_ERROR;

  if( iz < head[2] ) return PFC::E_PFC_ERROR;
  if( iz > tail[2] ) return PFC::E_PFC_ERROR;

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// ２次元固定長のアロケーション（整数用）
void
CPfcFunction::Alloc2D(
                  const int size1,  // [in]  １次元目サイズ
                  const int size2,  // [in]  ２次元目サイズ
                  int*  &data,      // [out] アロケーションされた領域（１次元連続領域）
                                    //           サイズ:size1*size2
                                    //           ポインタの参照型であることに注意
                                    //           使用後, delete [] data
                  int** &index      // [out] dataへのアクセス用インデックス
                                    //           [size1][size2]
                                    //           ダブルポインタの参照型であることに注意
                                    //           使用後, delete [] index
         )
{
  data = new int[size1*size2];

  index = new int*[size1];
  for(int i=0; i<size1; i++ ) {
    index[i] = (data+size2*i);
  }
}


// #################################################################
// ２次元固定長のアロケーション（倍精度）
void
CPfcFunction::Alloc2D(
                  const int size1,  // [in]  １次元目サイズ
                  const int size2,  // [in]  ２次元目サイズ
                  double*  &data,   // [out] アロケーションされた領域（１次元連続領域）
                                    //           サイズ:size1*size2
                                    //           ポインタの参照型であることに注意
                                    //           使用後, delete [] data
                  double** &index   // [out] dataへのアクセス用インデックス
                                    //           [size1][size2]
                                    //           ダブルポインタの参照型であることに注意
                                    //           使用後, delete [] index
         )
{
  data = new double[size1*size2];

  index = new double*[size1];
  for(int i=0; i<size1; i++ ) {
    index[i] = (data+size2*i);
  }
}


// #################################################################
// POD Max Layer数取得
int
CPfcFunction::GetPodMaxLayer(
                  const int numStep 
                )
{
  int max_layer;

  // 2**max_layer <= numStep の条件でmax_layerを求める
  max_layer = (int)( ( log(numStep)/log(2) ) ); // = (int)log2(numStep)

  return max_layer;
}

// #################################################################
// POD 並列数取得
int
CPfcFunction::GetPodParallel(
                  const int numStep 
                )
{
  int max_layer;
  int num_parallel;

  // 2**max_layer <= numStep の条件でmax_layerを求める
  max_layer = (int)( ( log(numStep)/log(2) ) + D_PFC_EPSILON ); // = (int)log2(numStep)

  // num_parralell = 2**(max_layer-1)
  num_parallel = (int)(pow(2, max_layer-1) + D_PFC_EPSILON );

  return num_parallel;
}

// #################################################################
// POD 担当するステップ数＆ステップ開始位置取得
PFC::E_PFC_ERRORCODE
CPfcFunction::GetPodStepInfo(
                  const int numStep,        // (in)  タイムステップ数
                  const int id,             // (in)  領域内のID(0からの連番）
                  int&      numStepInId,    // (out) idが担当するステップ数
                  int&      startStepPos,   // (out) idの開始ステップ位置(0からの連番）
                  int&      maxStep         // (out) 領域内の各ランクが担当する最大ステップ数
                )
{
  int max_layer;
  int num_step_base;


  // 2**max_layer <= numStep の条件でmax_layerを求める
  max_layer = (int)( ( log(numStep)/log(2) ) + D_PFC_EPSILON ); // = (int)log2(numStep)

  num_step_base = (int)(pow(2, max_layer) + D_PFC_EPSILON );
  
  if( numStep == num_step_base )  {
    numStepInId  = 2;
    startStepPos = 2*id;
    maxStep      = 2;

  } else {
    int n_234;
    int tag_n234;
    
    // num_parralell = 2**(max_layer-1)
    int num_parallel = num_step_base / 2;

    if( numStep > (num_step_base+num_step_base/2) ) {   // numStep 3 or 4
      tag_n234 = 3;
      maxStep  = 4;
    } else {                        // numStep 2 or 3
      tag_n234 = 2;
      maxStep  = 3;
    }
    
    n_234 = (tag_n234+1)*num_parallel - numStep;
    
    if( id < n_234 ) {
      numStepInId  = tag_n234;
      startStepPos = id*tag_n234;
    } else {
      numStepInId  = tag_n234+1;
      startStepPos = n_234*tag_n234 + (id-n_234)*(tag_n234+1);
    }
  }

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// POD のregionID（領域ID)取得
PFC::E_PFC_ERRORCODE
CPfcFunction::GetPodRegionID(
                  const int rankID,
                  const int numStep,
                  int&      regionID,
                  int&      idInRegion,
                  int&      mastarRankID
                )
{
  int numParallel = CPfcFunction::GetPodParallel( numStep );
                
  regionID     = rankID / numParallel;
  mastarRankID = regionID * numParallel;
  idInRegion   = rankID - mastarRankID;

  return PFC::E_PFC_SUCCESS;
}

  /**
   * @brief POD のbase index取得
   * @detail  base index: pod baseファイルを出力したランク番号
   * @param[in]   numStep             ステップ数
   * @param[out]  numParallel         並列数
   * @param[out]  numCalculatedLayer  計算レイヤー数
   *
   * @return base index
   */
// #################################################################
// POD のPOD のbase index取得
//     base index: pod baseファイルを出力したランク番号
int
CPfcFunction::GetPodBaseIndex(
                  const int numStep,            // [in]  numStep             ステップ数
                  const int numParallel,        // [in]  numParallel         並列数
                  const int numCalculatedLayer, // [in]  numCalculatedLayer  計算レイヤー数
                  const int stepID              // [in]  stepID              ステップID （0起点）
                )
{
  int base_index = -1;

  // 計算レイヤー数 1  Layer0
  if( numCalculatedLayer == 1 ) 
  {
    // 冗長な処理だがレアケースなので使用
    //   Layer0で圧縮が終わるケースは少ないと判断している
    for(int j=0; j<numParallel; j++) {
      int numStepInId, startStepPos, maxStep;

      CPfcFunction::GetPodStepInfo( numStep, j,
                                  numStepInId, startStepPos, maxStep );
      if( stepID >= startStepPos                 && 
          stepID <= (startStepPos+numStepInId-1)    )
      {
        base_index = j;
        break;
      }
    }
  }
  // Layer1以降はstepを2で割れば圧縮時のランク番号(=base_index)が分かる
  else if ( numCalculatedLayer > 1 )
  {
    base_index = stepID / 2;
  }

  return base_index;
}

