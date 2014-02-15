/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */


/**
 * @file   sample.cpp
 * @brief  performance monitor sample source
 * @author aics
 */

#include "mpi.h"
#include "PfcCompressCmd.h"
// PM ヘッダ
#include "PfcPerfMon.h"

int main( int argc, char **argv )
{
  std::string compressCntlPath = "./pfc_cntl";
  PFC::E_PFC_ERRORCODE ret;

  cout << "##### Compression Command start #####"<< endl;
	
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

  // PM 初期化
  PfcPerfMon::InitCompress( rankID );

  PfcPerfMon::Start(t1_c); // PM 区間測定開始

  CPfcCompressCmd compressCmd( compressCntlPath );

  PfcPerfMon::Stop(t1_c);  // PM 区間測定終了

  PfcPerfMon::Start(t2_c); // PM 区間測定開始

  ret = compressCmd.Execute();

  PfcPerfMon::Stop(t2_c);  // PM 区間測定終了

  if( ret == PFC::E_PFC_SUCCESS ) {
    cout << "##### Compression Command  Normal End  #####"<< endl;
  } else {
    cout << "##### Compression Command  Abormal End  #####"<< endl;
    cout << "   return code ="<<ret;
  }

  // PM 終了＆ファイル出力
  PfcPerfMon::Term();

  MPI_Finalize();

  return 0;
}


