/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */


/**
 * @file   main.cpp
 * @brief  sample of restraion in range
 * @author aics
 */

#include "mpi.h"
#include "PfcRestration.h"

int main( int argc, char **argv )
{
  PFC::E_PFC_ERRORCODE ret;

  cout << "##### Restration sample start #####"<< endl;

  CPfcRestration  restration;

  std::string pfcFilePath    = "IN_Compress/vel.pfc";

  // 初期化
  ret = restration.Init(
             pfcFilePath      // index.pfcのファイルパス
           );
  if( ret != PFC::E_PFC_SUCCESS ) {
    cout << "##### Restration Init() Error  #####"<< endl;
    return -1;
  }

  int head[3];
  int tail[3];

  // 計算領域の決定
  {
    int regionID  = 0; // 担当する領域ID
    int gDiv[3] = { 1, 1, 1 }; // 各方向の分割数

    ret = restration.GetHeadTail (
                      gDiv,     // [in]  計算空間の領域分割数
                      regionID, // [in]  領域ID
                      head,     // [out] 計算領域の開始位置
                      tail      // [out] 計算領域の終了位置
                 );
  }

  // タイムステップリスト取得
  vector<int> timeStepList;
  ret = restration.GetTimeStepList(
                       timeStepList // [out] タイムステップリスト
                     );

  if( ret != PFC::E_PFC_SUCCESS ) {
    cout << "##### Restration GetTimeStepList() Error  #####"<< endl;
    return -1;
  }
  int numStep = timeStepList.size();
  //int numStep = 2;  // 読みだすステップ数

  printf("##### Restration Result Dump Start #######\n");
      // 成分数 = 3
  // タイムステップループ
  for(int istep=0; istep<numStep; istep++ ) {
    printf("  ----- step = %d -------\n",istep);
    for(int iz=head[2]; iz<=tail[2]; iz++ ) {
      for(int iy=head[1]; iy<=tail[1]; iy++ ) {
        for(int ix=head[0]; ix<=tail[0]; ix++ ) {
          double dv[3];

          // データ読み込み（位置指定）
          ret = restration.ReadData ( 
                    dv,                  // [out] 読み込み領域
                    timeStepList[istep], // [in]  タイムステップ番号
                    ix,                  // [in]  x position ( >= 1 )
                    iy,                  // [in]  y position ( >= 1 )
                    iz                   // [in]  z position ( >= 1 )
               );
          if( ret != PFC::E_PFC_SUCCESS ) {
            cout << "##### Restration GetTimeStepList() Error  #####"<< endl;
            return -1;
          }

          int iwk = istep + iz + iy + ix;
          int ic;
          ic = 0;
          printf("istep=%3d ic=%3d iz=%3d iy=%3d ix=%3d orig=%d  dv=%15.4lf\n",
                  istep,ic,iz,iy,ix,(iwk+ic),dv[ic]);
          ic = 1;
          printf("istep=%3d ic=%3d iz=%3d iy=%3d ix=%3d orig=%d  dv=%15.4lf\n",
                  istep,ic,iz,iy,ix,(iwk+ic),dv[ic]);
          ic = 2;
          printf("istep=%3d ic=%3d iz=%3d iy=%3d ix=%3d orig=%d  dv=%15.4lf\n",
                  istep,ic,iz,iy,ix,(iwk+ic),dv[ic]);
        }
      }
    }

  } // ! タイムスタップループ
  printf("##### Restration Result Dump End  #######\n");

  cout << "##### Restration sample end #####"<< endl;


  return 0;
}

