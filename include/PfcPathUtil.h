// 必要がない、もしくはcio_PathUtil.hで代替できるのであれば、当ファイルは削除
//     展開用にはPFClibはくっつけたくないところ

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/**
 * @file   PfcPathUtil.h
 * @brief  Path utility関数
 * @author aics
 */

 //--------------------------------------
 // オリジナル: cio_PathUtil.h
 // 追加：MakeDirectory
 //--------------------------------------

#ifndef _PFC_PATHUTIL_H_
#define _PFC_PATHUTIL_H_

#include "PfcDefine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#define MAXPATHLEN 512

namespace PFC
{
  // デリミタ（char)の取得
  inline char PfcPath_getDelimChar()
  {
#ifdef WIN32
    return '\\';
#else
    return '/';
#endif
  }

  // デリミタ（文字列)の取得
  inline std::string PfcPath_getDelimString()
  {
    const char dc = PFC::PfcPath_getDelimChar();
    char rs[2] = {dc, '\0'};
    return rs;
  }

  // ドライブ名（例 C:)の有無
  inline bool PfcPath_hasDrive(const std::string& path) {
    if ( path.size() < 2 ) return false;
    char x = path[0];
    if ( ((x >= 'A' && x <= 'Z' ) || (x >= 'a' && x <= 'z')) &&
   path[1] == ':' )
      return true;
    return false;
  }

  // ドライブ名（例 C:)とドライブ名以外の文字の分離
  inline std::string vfvPath_emitDrive(std::string& path)
  {
    // returns drive (ex. 'C:')
    if ( ! PfcPath_hasDrive(path) ) return std::string();
    std::string driveStr = path.substr(0, 2);
    path = path.substr(2);
    return driveStr;
  }

  // true  : Absolute Path(絶対パス)
  // false : Relative Path(相対パス)
  inline bool PfcPath_isAbsolute(const std::string& path)
  {
    std::string xpath(path);
    vfvPath_emitDrive(xpath);
    char c1, c2;
    c1 = xpath[0];
    c2 = PfcPath_getDelimChar();
    return (c1 == c2);
  }

  // パスよりディレクトリ名の取得
  inline std::string PfcPath_DirName(const std::string& path,
                                     const char dc = PfcPath_getDelimChar()) {
    char* name = strdup( path.c_str() );
    char* p = name;

    for ( ; ; ++p ) {
      if ( ! *p ) {
        if ( p > name ) {
          char rs[2] = {dc, '\0'};
          return rs;
        } else {
          char rs[3] = {'.', dc, '\0'};
          return rs;
        }
      }
      if ( *p != dc ) break;
    }

    for ( ; *p; ++p );
    while ( *--p == dc ) continue;
    *++p = '\0';

    while ( --p >= name )
      if ( *p == dc ) break;
    ++p;
    if ( p == name )
    {
      char rs[3] = {'.', dc, '\0'};
      return rs;
    }

    while ( --p >= name )
      if ( *p != dc ) break;
    ++p;

    *p = '\0';
    if( p == name ) {
      char rs[2] = {dc, '\0'};
      return rs;
    } else {
      std::string s( name );
      free( name );
      if( !PFC::PfcPath_isAbsolute(s) )
      {
        const char *q = s.c_str();
        if( q[0] != '.' && q[1] != '/' )
        {
          char rs[3] = {'.', dc, '\0'};
          s = std::string(rs) + s;
        }
      }
      return s;
    }
  }

  // パスよりファイル名の取出し、拡張子がない場合は拡張子を付ける
  inline std::string PfcPath_FileName(const std::string& path,
                                      const std::string& addext = std::string(""),
                                      const char dc = PfcPath_getDelimChar()) {
    char* name = strdup( path.c_str() );
    char* p = name;

    for ( ; ; ++p ) {
      if ( ! *p ) {
        if ( p > name ) {
          char rs[2] = {dc, '\0'};
          return rs;
        } else
          return "";
      }
      if ( *p != dc ) break;
    }

    for ( ; *p; ++p ) continue;
    while ( *--p == dc ) continue;
    *++p = '\0';

    while ( --p >= name )
      if ( *p == dc ) break;
    ++p;

    bool add = false;
    if ( addext.length() > 0 ) {
      const int suffixlen = addext.length();
      const int stringlen = strlen( p );
      if ( suffixlen < stringlen ) {
        const int off = stringlen - suffixlen;
        if ( strcasecmp( p + off, addext.c_str()) != 0 )
          add = true;
      }
      else
      {
          add = true;
      }
    }

    std::string s( p );
    if( add ) s += addext;

    free( name );
    return s;
  }

  // ディレクトリ名とファイル名の連結
  inline std::string PfcPath_ConnectPath( std::string dirName, std::string fname )
  {
    std::string path = dirName;

    const char *p = dirName.c_str();
    if( p[strlen(p)-1] != PFC::PfcPath_getDelimChar() )
    {
      path += PFC::PfcPath_getDelimString();
    }

    path += fname;

    return path;
  }
  
  // ディレクトリがなければ作成、既存なら何もしない
  PFC::E_PFC_ERRORCODE
  MakeDirectory( const std::string& path );

};

#endif /* _PFC_PATHUTIL_H_ */

