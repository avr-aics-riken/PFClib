/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */


/**
 * @file   main.cpp
 * @brief  sample of compress
 * @author aics
 */

#include "mpi.h"
#include "PfcCompress.h"

int main( int argc, char **argv )
{
  PFC::E_PFC_ERRORCODE ret;

	
  //MPI Initialize
  if( MPI_Init(&argc,&argv) != MPI_SUCCESS )
  {
      std::cerr << "MPI_Init error." << std::endl;
      return false;
  }
  int numRank;
  int rankID;
  MPI_Comm_size( MPI_COMM_WORLD, &numRank );
  MPI_Comm_rank( MPI_COMM_WORLD, &rankID );

  if( rankID == 0 ) {
    cout << "##### Compression sample start #####"<< endl;
  }

  CPfcCompress  compress;

  std::string dfiFilePath    = "IN_Cio/vel.dfi";
  std::string outDirPath     = "field_data";
  std::string compressFormat = "pod";
  //double      compressError  = 3.0;  // 誤差率(%)
  double      compressError  = 1.0;  // 誤差率(%)
  int         domainDivision[3];
  int         startStep      = 0;
  int         endStep        = 30; int numStep=4;
  int         optFlags       = 0;

  //ステップ数よりステップ方向の圧縮並列数を取得
  int numParallel = CPfcFunction::GetPodParallel( numStep );
  int numRegion   = numRank/numParallel;

  if( numRegion == 1 ) {
    domainDivision[0] = 1; domainDivision[1] = 1; domainDivision[2] = 1;
  } else if( numRegion == 2 ) {
    domainDivision[0] = 2; domainDivision[1] = 1; domainDivision[2] = 1;
  } else if( numRegion == 4 ) {
    domainDivision[0] = 2; domainDivision[1] = 2; domainDivision[2] = 1;
  } else if( numRegion == 8 ) {
    domainDivision[0] = 2; domainDivision[1] = 2; domainDivision[2] = 2;
  } else if( numRegion == 16 ) {
    domainDivision[0] = 4; domainDivision[1] = 2; domainDivision[2] = 2;
  } else {
    domainDivision[0] = 2; domainDivision[1] = 2; domainDivision[2] = 2;
  }

  // 初期化
  if( rankID == 0 ) {
    cout << "##### Compression Init()  start #####"<< endl;

    cout << "  numRank     = "<<numRank<< endl;
    cout << "  rankID      = "<<rankID<< endl;
    cout << "  numStep     = "<<numStep<< endl;
    cout << "  numParallel = "<<numParallel<< endl;
    cout << "  numRegion   = "<<numRegion<< endl;
    cout << "  domainDivision[3] = "<<domainDivision[0]<<" "<<domainDivision[1]<<" "<<domainDivision[2]<< endl;
    cout << "  compressError     = "<<compressError<< endl;
  }

  ret = compress.Init(
             MPI_COMM_WORLD,  // MPIコミュニケータ
             dfiFilePath,     // DFIファイルパス
             outDirPath,      // フィールドデータ出力ディレクトリパス 
             compressFormat,  // 圧縮フォーマット
             compressError,   // 誤差率(%)
             domainDivision,  // 領域の分割数
             startStep,       // 開始ステップ
             endStep,         // 終了ステップ
             optFlags         // オプションflags
           );
  if( ret != PFC::E_PFC_SUCCESS ) {
    cout << "##### Compression Init() Error  #####"<< endl;
    return -1;
  }

  // proc.pfcファイル出力
  if( rankID == 0 ) {
    cout << "##### Compression WriteProcFile()  start #####"<< endl;
  }

  ret = compress.WriteProcFile();
  if( ret != PFC::E_PFC_SUCCESS ) {
    cout << "##### Compression WriteProcFile() Error  #####"<< endl;
    return -1;
  }

  // 圧縮＆圧縮データファイル＆index.pfc出力処理
  if( rankID == 0 ) {
    cout << "##### Compression compress.WriteData()  start #####"<< endl;
  }

  ret = compress.WriteData();
  if( ret != PFC::E_PFC_SUCCESS ) {
    cout << "##### Compression WriteProcFile() Error  #####"<< endl;
    return -1;
  }

  if( rankID == 0 ) {
    cout << "##### Compression sample end #####"<< endl;
  }

  MPI_Finalize();

  return 0;
}


