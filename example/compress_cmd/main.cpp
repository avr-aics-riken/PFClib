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

  CPfcCompressCmd compressCmd( compressCntlPath );

  ret = compressCmd.Execute();

  if( ret == PFC::E_PFC_SUCCESS ) {
    cout << "##### Compression Command  Normal End  #####"<< endl;
  } else {
    cout << "##### Compression Command  Abormal End  #####"<< endl;
    cout << "   return code ="<<ret;
  }

  MPI_Finalize();

  return 0;
}


