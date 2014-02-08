#ifndef _PFC_MPI_STUBS_H_
#define _PFC_MPI_STUBS_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/**
 * @file   mpi_stub.h
 * @brief  stub for serial
 */

typedef int MPI_Comm;
typedef int MPI_Datatype;
#define MPI_COMM_WORLD 0
#define MPI_INT  1
#define MPI_CHAR 2

#define MPI_SUCCESS true

inline bool MPI_Init(int* argc, char*** argv) { return true; }

inline int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
  *rank = 0;
  return 0;
}

inline int MPI_Comm_size(MPI_Comm comm, int *size)
{
  *size = 1;
  return 0;
}

inline int MPI_Allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                         void *recvbuf, int recvcount, MPI_Datatype recvtype,
                         MPI_Comm comm)
{
  return 0;
}

inline int MPI_Gather(void *sendbuf, int sendcnt, MPI_Datatype sendtype,
                      void *recvbuf, int recvcnt, MPI_Datatype recvtype,
                      int root, MPI_Comm comm)
{
  return 0;
}




#endif /* _PFC_MPI_STUBS_H_ */
