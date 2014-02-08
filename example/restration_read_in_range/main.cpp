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
#include "cio_DFI.h"
#include "PfcRestration.h"

#define ERROR               0.000001   

double speed_length(double x, double y, double z){
  return(sqrt(x*x+y*y+z*z));
}

//****************************************************
int main( int argc, char **argv )
{
  PFC::E_PFC_ERRORCODE ret;

  cout << "##### Restration sample start #####"<< endl;

  //MPI Initialize  for CIO
  if( MPI_Init(&argc,&argv) != MPI_SUCCESS )
  {
      std::cerr << "MPI_Init error." << std::endl;
      return false;
  }

  CPfcRestration  restration;

  std::string pfcFilePath    = "IN_Compress/vel.pfc";
  std::string dfiFilePath    = "IN_Cio/vel.dfi";


  // PFClib 展開処理初期化
  ret = restration.Init(
             pfcFilePath      // index.pfcのファイルパス
           );
  if( ret != PFC::E_PFC_SUCCESS ) {
    cout << "##### Restration Init() Error  #####"<< endl;
    return -1;
  }

  int head[3];
  int tail[3];
  int numRegion = 1; // 全領域数
  int regionID  = 0; // 担当する領域ID
  int gDiv[3]   = { 1, 1, 1 }; // 各方向の分割数
  int gVoxel[3]; // 各方向のvoxel数

  // 計算領域の決定
  {
    restration.GetGlobalVoxel( gVoxel );

    ret = restration.GetHeadTail (
                      gDiv,     // [in]  計算空間の領域分割数
                      regionID, // [in]  領域ID
                      head,     // [out] 計算領域の開始位置
                      tail      // [out] 計算領域の終了位置
                 );
    printf("head[3] = %d %d %d\n",head[0],head[1],head[2]);
    printf("tail[3] = %d %d %d\n",tail[0],tail[1],tail[2]);
  }

  // CIO初期化
  int guideCell = 0;
  double  f_time;
  unsigned int i_dummy;
  double  f_dummy;
  CIO::E_CIO_ERRORCODE ret_cio;

  cio_DFI* pDfiIN = cio_DFI::ReadInit(
                        MPI_COMM_WORLD,
                        dfiFilePath,
                        gVoxel,
                        gDiv,
                        ret_cio );
            

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

  // 領域アロケート
  int size = (tail[0]-head[0]+1)*(tail[1]-head[1]+1)*(tail[2]-head[2]+1);
  int numComponent = 3;   // 成分数
     // CIO Sph original data 
  double* dv_orig = new double[size*numComponent*numStep];

  double* dv = new double[size*numComponent*numStep];


  // タイムステップループ
  for(int istep=0; istep<numStep; istep++ ) {
    int ip = size*numComponent*istep;

    // PFC Podデータ読み込み（範囲指定）
    ret = restration.ReadData ( 
                &dv[ip],             // [out] 読み込み領域先頭
                timeStepList[istep], // [in]  タイムステップ番号
                head,                // [in]  計算領域の開始位置
                tail                 // [in]  計算領域の終了位置
               );
    if( ret != PFC::E_PFC_SUCCESS ) {
      cout << "##### restration.ReadData() Error ret="<<ret<<"  #####"<< endl;
      return -1;
    }

    // CIO Sph originalデータ読み込み

    pDfiIN->ReadData(
                    &dv_orig[ip],
                    timeStepList[istep],
                    guideCell,
                    gVoxel,
                    gDiv,
                    head,
                    tail,
                    f_time,
                    true,
                    i_dummy,
                    f_dummy
                );

  } // ! タイムスタップループ

  // 検証
  int ix_size = (tail[0]-head[0]+1);
  int iy_size = (tail[1]-head[1]+1);
  int iz_size = (tail[2]-head[2]+1);
  int nsize   = ix_size*iy_size*iz_size;
  double max_abs_diff = 0.0;
  
  // NIJKの場合のダンプ出力
  printf("##### Restration Result Dump Start #######\n");
  for(int istep=0; istep<numStep; istep++ ) {
  printf("  ----- step = %d -------\n",istep);
    for(int iz=head[2]; iz<=tail[2]; iz++ ) {
      for(int iy=head[1]; iy<=tail[1]; iy++ ) {
        for(int ix=head[0]; ix<=tail[0]; ix++ ) {
          for(int ic=0; ic<numComponent; ic++ ) {
            int ip =   numComponent*ix_size*iy_size*iz_size*istep
                     + numComponent*ix_size*iy_size*(iz-head[2])
                     + numComponent*ix_size*(iy-head[1])
                     + numComponent*(ix-head[0])
                     + ic;

            double diff = dv_orig[ip] - dv[ip];
            printf("istep=%3d ic=%3d iz=%3d iy=%3d ix=%3d  orig=%15.4lf  val=%15.4lf  diff=%15.4lf\n",
                    istep,ic,iz,iy,ix,dv_orig[ip],dv[ip],diff);
            if( fabs(diff) > max_abs_diff ) {
              max_abs_diff = fabs(diff);
            }
          }
        }
      }
    }
  }
  printf("\n");
  printf("**** max_abs_diff =%15.4lf\n",max_abs_diff);

  double* error_co = new double[nsize];
  double* error    = new double[numStep];
  double* sum_orig = new double[numStep];
  double* fc       = new double[numStep];
  double* largest_error = new double[numStep];
  double* sum_compression = new double[numStep];
  double temp_data1, temp_data2;
  int    ip;

  for(int i = 0 ; i < numStep; i++){
    sum_orig[i] = 0.0;
    sum_compression[i] = 0.0;
    largest_error[i] = -1.0;
    fc[i] = 0.0;
  }

  for(int istep=0; istep<numStep; istep++ ) {
    double average = 0.0;
    double fangcha = 0.0;
    double ave1 = 0.0;
    double ave2 = 0.0;
    double ave = 0.0;
    double temp;

    for(int i=0; i<nsize; i++ ) {
      ip = nsize*istep + numComponent*i;
      temp_data1 = speed_length(dv_orig[ip], dv_orig[ip+1], dv_orig[ip+2]);
      temp_data2 = speed_length(dv[ip], dv[ip+1], dv[ip+2]);
	  
      if(fabs(temp_data1) < ERROR ) {
        temp = 0.0;
      } else {
        temp = 1.0/(temp_data1*temp_data1);
      }
      ave1 = ave1 + (temp_data1 - temp_data2)*temp;
      ave2 = ave2 + temp;

    }
    ave = ave1/ave2;	  

    for(int i=0; i<nsize; i++ ) {
      ip = nsize*istep + numComponent*i;
      temp_data1 = speed_length(dv_orig[ip], dv_orig[ip+1], dv_orig[ip+2]);
      temp_data2 = speed_length(dv[ip], dv[ip+1], dv[ip+2]) + ave;

      if(fabs(temp_data1) < ERROR ) {
      	error_co[i] = 0.0;
      } else {
        error_co[i] = fabs(temp_data1-temp_data2)/temp_data1/nsize;
      	average	= average + error_co[i];
      }
    }


    for(int i=0; i<nsize; i++ ) {
      ip = nsize*istep + numComponent*i;
      temp_data1 = speed_length(dv_orig[ip], dv_orig[ip+1], dv_orig[ip+2]);
      temp_data2 = speed_length(dv[ip], dv[ip+1], dv[ip+2]) + ave;

      if(fabs(temp_data1) < ERROR ){
        temp_data1 = 0.0;
        temp_data2 = 0.0;
      }

      sum_orig[istep] = sum_orig[istep] + temp_data1/nsize;
      sum_compression[istep] = sum_compression[istep] + fabs(temp_data1-temp_data2)/nsize; 
    }

    error[istep] = sum_compression[istep]/sum_orig[istep]*100.0;
    for(int i = 0; i < nsize; i++) {
      fangcha = fangcha + pow(error_co[i]*nsize-average,2);
    }
    fangcha = fangcha / nsize;
    fangcha = sqrt(fangcha);
    fc[istep] = fangcha;

    cout<<endl;
    cout<<"#### finish the "<<istep<<"th timestep ######"<<endl;
    cout<<"   error:"<<error[istep]<<"  fangcha:"<<fangcha<<endl;
    cout<<endl;     
  }

  delete [] dv;
  delete [] dv_orig;
  delete [] error_co;
  delete [] error;
  delete [] sum_orig;
  delete [] fc;
  delete [] largest_error;
  delete [] sum_compression;
  delete pDfiIN;

  // CIO 
  MPI_Finalize();

  cout << "##### Restration sample end #####"<< endl;


  return 0;
}



