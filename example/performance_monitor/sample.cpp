/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */


/**
 * @file   main.cpp
 * @brief  compress command main関数
 * @author aics
 */

#include "mpi.h"
#include "PfcCompressCmd.h"
#include "PfcPerfMon.h"

int main( int argc, char **argv )
{
  std::string compressCntlPath = "./pfc_cntl";
  PFC::E_PFC_ERRORCODE ret;
  const int n_thread = 1;
	
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
    cout << "##### Compression Command start #####"<< endl;
  }

  PfcPerfMon::InitCompress( numRank, rankID, n_thread );

  CPfcCompressCmd compressCmd( compressCntlPath );

  ret = compressCmd.Execute();

  if( ret == PFC::E_PFC_SUCCESS ) {
    if( rankID == 0 ) {
      cout << "##### Compression Command  Normal End  #####"<< endl;
    }
  } else {
    cout << "##### Compression Command  Abormal End  #####"<< endl;
    cout << "   return code ="<<ret;
  }

  PfcPerfMon::Term();

  MPI_Finalize();

  return 0;
}

