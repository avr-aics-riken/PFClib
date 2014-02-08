/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcPathUtil.cpp 
 * @brief  Path utility関数
 * @author aics    
 */

#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <typeinfo>
#include "PfcPathUtil.h"

static int MakeDirectorySub( const std::string& path );


// #################################################################
// ディレクトリがなければ作成、既存なら何もしない
PFC::E_PFC_ERRORCODE
PFC::MakeDirectory( const std::string& path )
{
  int ret = MakeDirectorySub(path);
  if( ret != 0 )
  {
     // 既存以外のエラー
     if ( EEXIST != errno )
     {
        PFC_PRINT( "\tError(errno)=[%s]\n", strerror(errno) );
        return PFC::E_PFC_ERROR;
     }
  }

  return PFC::E_PFC_SUCCESS;       
}

//#################################################################
// 内部関数
//#################################################################

// 戻り値： 0 正常  0以外 エラー
int MakeDirectorySub( const std::string& path )
{

  umask(022);

  int ret = mkdir(path.c_str(), 0777);
  if( ret != 0 )
  {
    if( errno == EEXIST ) return 0;

    std::string parent = PFC::PfcPath_DirName(path);
    int ret2 = MakeDirectorySub( parent );
    if( ret2 != 0 )
    {
      return ret2;
    }
    ret = MakeDirectorySub( path );
  }
  else
  {
    ret = 0;
  }

  return ret;
}
