/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcPodFile.cpp
 * @brief  CPfcPodFile Class
 * @author aics
 */

#include "PfcPodFile.h"
#include "PfcPathUtil.h"
#include "PfcEndianChange.h"


// #################################################################
// 基底ファイル出力処理
PFC::E_PFC_ERRORCODE
CPfcPodFile::WriteBaseFile(
                const std::string&  outDirPath,         // (in) 出力ディレクトリ
                const std::string&  prefix,             // (in) 属性名
                const int           regionID,           // (in) 領域ID
                const bool          bSingle,            // (in) 単一領域フラグ
                const int           numStep,            // (in) タイムステップ数
                const int           numParallel,        // (in) 圧縮時並列数
                const int           numCalculatedLayer, // (in) 圧縮時計算レイヤー数
                const int*          nSizes,             // (in) 各ランク内の要素数
                double*             pPod_base           // (in) 基底データ
            )
{
  // ファイル名の生成
  char file_name[512];
  std::string fileName;

  if( bSingle ) {
    sprintf( file_name,"%s.pod",prefix.c_str() );
  } else {
    sprintf( file_name,"%s_id%06d.pod",prefix.c_str(),regionID );
  }
  std::string sFileName( file_name );

  std::string sFilePath = PFC::PfcPath_ConnectPath( outDirPath, sFileName);

  ofstream ofs1(sFilePath.c_str(), ios::out | ios::binary);
  if (!ofs1) { 
    PFC_PRINT( "### ERROR ### Can't open filename=%s\n",sFilePath.c_str() );
    return PFC::E_PFC_ERROR_WRITE_PODBASE_OPENERROR;
  }

  int endian_check = 1;
  int data_type    = 2;
  int total_pod_size = 0;
  
  for(int i=0; i<numParallel; i++ ) {
    total_pod_size += nSizes[i];
  }
  
  ofs1.write((char*)&endian_check, sizeof(int) );       // エンディアンチェック
  ofs1.write((char*)&data_type, sizeof(int) );          // データタイプ
  ofs1.write((char*)&numStep, sizeof(int) );            // タイムステップ数
  ofs1.write((char*)&numParallel, sizeof(int) );        // 圧縮時並列数
  ofs1.write((char*)&numCalculatedLayer, sizeof(int) ); // 圧縮時計算レイヤー数
  ofs1.write((char*)nSizes, sizeof(int)*numParallel );  // 各ランク内の要素数

  ofs1.write((char*)pPod_base, sizeof(double)*total_pod_size);  // 基底データ

  if( ofs1.bad() ) {
    return PFC::E_PFC_ERROR_WRITE_PODBASE_DATA;
  }

  ofs1.close();

  return PFC::E_PFC_SUCCESS;
}

// #################################################################
// 基底ファイル出力処理（デバッグ用）
//     途中のレイヤーの基底ファイルを出力する
PFC::E_PFC_ERRORCODE
CPfcPodFile::WriteBaseFile_Debug(
                const std::string&  outDirPath,         // (in) 出力ディレクトリ
                const std::string&  prefix,             // (in) 属性名
                const int           regionID,           // (in) 領域ID
                const int           idInRregion,        // (in) 領域内のID
                const int           layerNo,            // (in) レイヤーNo （0起点）
                const int           numStep,            // (in) タイムステップ数
                const int           numParallel,        // (in) 圧縮時並列数
                const int           numCalculatedLayer, // (in) 圧縮時計算レイヤー数
                                                        //       layerNo:numCalculatedLayer-1
                const int           nSize,              // (in) 各ランクの要素数
                double*             pPod_base           // (in) 各ランクの基底データ
            )
{
  if( layerNo != (numCalculatedLayer-1) ) {
    PFC_PRINT( "### ERROR ### Layer No error  layerNo=%d  numCalculatedLayer=%d\n",
                     layerNo, numCalculatedLayer );
    return PFC::E_PFC_ERROR;
  }

  // ファイル名の生成
  char file_name[512];
  std::string fileName;

  sprintf( file_name,"%s_id%06d-%05d_layer%02d.pod",
                     prefix.c_str(),regionID,idInRregion,layerNo );
  std::string sFileName( file_name );

  std::string sFilePath = PFC::PfcPath_ConnectPath( outDirPath, sFileName);

  ofstream ofs1(sFilePath.c_str(), ios::out | ios::binary);
  if (!ofs1) {
    PFC_PRINT( "### ERROR ### Can't open filename=%s\n",sFilePath.c_str() );
    return PFC::E_PFC_ERROR_WRITE_PODBASE_OPENERROR;
  }

  int endian_check = 1;
  int data_type    = 2;

  ofs1.write((char*)&endian_check, sizeof(int) );       // エンディアンチェック
  ofs1.write((char*)&data_type, sizeof(int) );          // データタイプ
  ofs1.write((char*)&numStep, sizeof(int) );            // タイムステップ数
  ofs1.write((char*)&numParallel, sizeof(int) );        // 圧縮時並列数
  ofs1.write((char*)&numCalculatedLayer, sizeof(int) ); // 圧縮時計算レイヤー数
  ofs1.write((char*)&nSize, sizeof(int) );              // 各ランクの要素数

  ofs1.write((char*)pPod_base, sizeof(double)*nSize);   // 各ランクの基底データ

  if( ofs1.bad() ) {
    return PFC::E_PFC_ERROR_WRITE_PODBASE_DATA;
  }

  ofs1.close();

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 基底ファイル入力（読み込み）処理
//   open/read/close
PFC::E_PFC_ERRORCODE
CPfcPodFile::ReadBaseFile(
                const std::string&  dirPath,            // (in)  入力ディレクトリ
                const std::string&  prefix,             // (in)  属性名
                const int           regionID,           // (in)  領域ID
                const bool          bSingle,            // (in) 単一領域フラグ
                int&                numStep,            // (out) タイムステップ数
                int&                numParallel,        // (out) 圧縮時並列数
                int&                numCalculatedLayer, // (out) 圧縮時計算レイヤー数
                int*                &pSizes,            // (out) 各ランク内の要素数
                                                        //          ポインタの参照型であることに注意
                                                        //          使用後, delete [] pSizes 必要
                double*             &pPod_base,         // (out) 基底データ
                                                        //          ポインタの参照型であることに注意
                                                        //          使用後, delete [] pPod_base 必要
                double**            &pIndex             // (out) 各ランク内の基底データへのポインタ
                                                        //          ポインタの参照型であることに注意
                                                        //          使用後, delete [] pIndex 必要
            )
{
  PFC::E_PFC_ERRORCODE ret;

  pSizes    = NULL;
  pPod_base = NULL;
  pIndex    = NULL;

  // ファイルOpen
  ifstream ifs;

  ret = CPfcPodFile::OpenBaseFile(
                dirPath,            // (in)  入力ディレクトリ
                prefix,             // (in)  属性名
                regionID,           // (in)  領域ID
                bSingle,            // (in) 単一領域フラグ
                ifs                 // (out) 入力ファイルストリーム
            );

  if (ret != PFC::E_PFC_SUCCESS) { 
    return PFC::E_PFC_ERROR_READ_PODBASE_OPENERROR;
  }

  // Header部呼び出し
  bool endian_chg;
  int  total_header_bsize;
  int  total_base_size;
  
  ret = CPfcPodFile::ReadBaseFileHeader(
                        ifs,                // (in) 入力ファイルストリーム
                        numStep,            // (out) タイムステップ数
                        numParallel,        // (out) 圧縮時並列数
                        numCalculatedLayer, // (out) 圧縮時計算レイヤー数
                        pSizes,             // (out) 各ランク内の要素数
                        total_header_bsize, // (out) ヘッダ部のトータルサイズ(byte)
                        total_base_size,    // (out) 基底データのトータルサイズ
                        endian_chg          // (out) エンディアン変換フラグ
            );
  if (ret != PFC::E_PFC_SUCCESS) { 
    return PFC::E_PFC_ERROR_READ_PODBASE_HEADER;
  }
  
  // 基底データ部の読み出し
  pPod_base  = new double[total_base_size];
  ifs.read((char*)pPod_base, sizeof(double)*total_base_size);  // 基底データ
  
  if( ifs.bad() ) {
    return PFC::E_PFC_ERROR_READ_PODBASE_DATA;
  }
  
  if( endian_chg ) {
    for(int i=0; i<total_base_size; i++ ) {
      endswap(&pPod_base[i]);
    }
  }

  // Index作成
  pIndex = new double*[numParallel];
  int total_size = 0;
  for(int i=0; i<numParallel; i++ ) {
    pIndex[i] = (pPod_base+total_size);
    total_size += pSizes[i];
  }

  // ファイルClose
  CPfcPodFile::CloseBaseFile( ifs );

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 基底ファイルOpen処理(read mode)
PFC::E_PFC_ERRORCODE
CPfcPodFile::OpenBaseFile(
                const std::string&  dirPath,            // (in)  入力ディレクトリ
                const std::string&  prefix,             // (in)  属性名
                const int           regionID,           // (in)  領域ID
                const bool          bSingle,            // (in) 単一領域フラグ
                ifstream&           ifs                 // (out) 入力ファイルストリーム
            )
{
  // ファイル名の生成
  char file_name[512];
  std::string fileName;

  if( bSingle ) {
    sprintf( file_name,"%s.pod",prefix.c_str() );
  } else {
    sprintf( file_name,"%s_id%06d.pod",prefix.c_str(),regionID );
  }
  std::string sFileName( file_name );

  std::string sFilePath = PFC::PfcPath_ConnectPath( dirPath, sFileName);

  ifs.open(sFilePath.c_str(), ios::in | ios::binary);
  if (!ifs) { 
    PFC_PRINT( "### ERROR ### Can't open filename=%s\n",sFilePath.c_str() );
    return PFC::E_PFC_ERROR_READ_PODBASE_OPENERROR;
  }

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 基底ファイルClose処理
void
CPfcPodFile::CloseBaseFile(
                ifstream&           ifs                 // (in) 入力ファイルストリーム
            )
{
  ifs.close();
}


// #################################################################
// 基底ファイルヘッダ部読み込み処理
PFC::E_PFC_ERRORCODE
CPfcPodFile::ReadBaseFileHeader(
                ifstream&           ifs,                // (in) 入力ファイルストリーム
                int&                numStep,            // (out) タイムステップ数
                int&                numParallel,        // (out) 圧縮時並列数
                int&                numCalculatedLayer, // (out) 圧縮時計算レイヤー数
                int*                &pSizes,            // (out) 各ランク内の要素数
                                                        //          ポインタの参照型であることに注意
                                                        //          使用後, delete [] pSizes 必要
                int&                total_header_bsize, // (out) ヘッダ部のトータルサイズ(byte)
                int&                total_base_size,    // (out) 基底データのトータルサイズ
                bool&               endian_chg          // (out) エンディアン変換フラグ
            )
{
  int  endian_check_compare = 1;
  int  endian_check;
  int  data_type;

  pSizes    = NULL;
  total_header_bsize = 0;

  ifs.read((char*)&endian_check, sizeof(int) );         // エンディアンチェック
  if( endian_check == endian_check_compare ) {
    endian_chg = false;
  } else {
    endian_chg = true;
  }
  total_header_bsize += sizeof(int);
  
  ifs.read((char*)&data_type, sizeof(int) );            // データタイプ
  if( endian_chg ) endswap(&data_type);
  total_header_bsize += sizeof(int);

  ifs.read((char*)&numStep, sizeof(int) );              // タイムステップ数
  if( endian_chg ) endswap(&numStep);
  total_header_bsize += sizeof(int);

  ifs.read((char*)&numParallel, sizeof(int) );          // 圧縮時並列数
  if( endian_chg ) endswap(&numParallel);
  total_header_bsize += sizeof(int);

  ifs.read((char*)&numCalculatedLayer, sizeof(int) ); // 圧縮時計算レイヤー数
  if( endian_chg ) endswap(&numCalculatedLayer);
  total_header_bsize += sizeof(int);

  pSizes = new int[numParallel];
  ifs.read((char*)pSizes, sizeof(int)*numParallel );    // 各ランク内の要素数
  total_header_bsize += (sizeof(int)*numParallel);

  total_base_size = 0;
  if( endian_chg ) {
    for(int i=0; i<numParallel; i++ ) {
      endswap(&pSizes[i]);
      total_base_size += pSizes[i];
    }
  } else {
    for(int i=0; i<numParallel; i++ ) {
      total_base_size += pSizes[i];
    }
  }

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 基底データ（１成分）読み込み処理
PFC::E_PFC_ERRORCODE
CPfcPodFile::ReadBaseFile1Elm(
                ifstream&   ifs,                // (in)  入力ファイルストリーム
                bool        endian_chg,         // (in)  エンディアン変換フラグ
                int         total_header_bsize, // (in)  ヘッダ部のトータルサイズ(byte)
                int         index,              // (in)  基底データ部の読み込みインデックス
                double&     v                   // (out) 基底データ(１成分)
            )
{
  int seek_size = total_header_bsize + sizeof(double)*index;
  
  // ファイルの先頭を基準としてseek
  ifs.seekg( seek_size, std::ios::beg );
  
  // 1要素読み込み
  ifs.read( (char*)(&v), sizeof(double) );

  if( ifs.bad() ) {
    return PFC::E_PFC_ERROR_READ_PODBASE_DATA;
  }

  // エンディアン変換
  if( endian_chg ) endswap(&v);

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 基底ファイル入力（読み込み）処理（デバッグ用）
//   デバッグ用の基底ファイルを読み込む
PFC::E_PFC_ERRORCODE
CPfcPodFile::ReadBaseFile_Debug(
                const std::string&  dirPath,            // (in)  入力ディレクトリ
                const std::string&  prefix,             // (in)  属性名
                const int           regionID,           // (in)  領域ID
                const int           idInRregion,        // (in)  領域内のID
                const int           layerNo,            // (in)  レイヤーNo （0起点）
                int&                numStep,            // (out) タイムステップ数
                int&                numParallel,        // (out) 圧縮時並列数
                int&                numCalculatedLayer, // (out) 圧縮時計算レイヤー数
                int&                nSize,              // (out) 各ランクの要素数
                double*             &pPod_base          // (out) 各ランクの基底データ
                                                        //          ポインタの参照型であることに注意
                                                        //          使用後, delete [] pPod_base 必要
            )
{
  pPod_base = NULL;

  // ファイル名の生成
  char file_name[512];
  std::string fileName;

  sprintf( file_name,"%s_id%06d-%05d_layer%02d.pod",
                     prefix.c_str(),regionID,idInRregion,layerNo );
  std::string sFileName( file_name );

  std::string sFilePath = PFC::PfcPath_ConnectPath( dirPath, sFileName);

  // ファイルOpen
  ifstream ifs;
  ifs.open(sFilePath.c_str(), ios::in | ios::binary);
  if (!ifs) {
    PFC_PRINT( "### ERROR ### Can't open filename=%s\n",sFilePath.c_str() );
    return PFC::E_PFC_ERROR_READ_PODBASE_OPENERROR;
  }

  // ヘッダ部読み込み処理
  bool endian_chg;
  int  endian_check_compare = 1;
  int  endian_check;
  int  data_type;

  ifs.read((char*)&endian_check, sizeof(int) );         // エンディアンチェック
  if( endian_check == endian_check_compare ) {
    endian_chg = false;
  } else {
    endian_chg = true;
  }

  ifs.read((char*)&data_type, sizeof(int) );            // データタイプ
  if( endian_chg ) endswap(&data_type);

  ifs.read((char*)&numStep, sizeof(int) );              // タイムステップ数
  if( endian_chg ) endswap(&numStep);

  ifs.read((char*)&numParallel, sizeof(int) );          // 圧縮時並列数
  if( endian_chg ) endswap(&numParallel);

  ifs.read((char*)&numCalculatedLayer, sizeof(int) ); // 圧縮時計算レイヤー数
  if( endian_chg ) endswap(&numCalculatedLayer);

  ifs.read((char*)&nSize, sizeof(int) );             // 各ランク内の要素数
  if( endian_chg ) endswap(&nSize);

  // 基底データ部の読み出し
  pPod_base  = new double[nSize];
  ifs.read((char*)pPod_base, sizeof(double)*nSize);  // 基底データ

  if( ifs.bad() ) {
    return PFC::E_PFC_ERROR_READ_PODBASE_DATA;
  }

  if( endian_chg ) {
    for(int i=0; i<nSize; i++ ) {
      endswap(&pPod_base[i]);
    }
  }

  // ファイルClose
  CPfcPodFile::CloseBaseFile( ifs );

  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// 係数ファイル出力処理
PFC::E_PFC_ERRORCODE
CPfcPodFile::WriteCoefFile(
                const std::string&  outDirPath,         // (in) 出力ディレクトリ
                const std::string&  prefix,             // (in) 属性名
                const int           regionID,           // (in) 領域ID (<0: 領域分割なし)
                const bool          bSingle,            // (in) 単一領域フラグ
                const int           numStep,            // (in) タイムステップ数
                                                        //          == Layer0の係数出力数
                const int           numCoef,            // (in) 係数出力数（Layer1以降）
                const int           numCalculatedLayer, // (in) 圧縮時計算レイヤー数
                double*             pCoef_a_layer       // (in) 係数データ
                                                        //         [numCalculatedLayer][numStep];
            )
{
  // ファイル名の生成
  char file_name[512];
  std::string fileName;

  if( bSingle ) {
    sprintf( file_name,"%s.coef",prefix.c_str() );
  } else {
    sprintf( file_name,"%s_id%06d.coef",prefix.c_str(),regionID );
  }
  std::string sFileName( file_name );

  std::string sFilePath = PFC::PfcPath_ConnectPath( outDirPath, sFileName);

  ofstream ofs1(sFilePath.c_str(), ios::out | ios::binary);
  if (!ofs1) { 
    PFC_PRINT( "### ERROR ### Can't open filename=%s\n",sFilePath.c_str() );
    return PFC::E_PFC_ERROR_WRITE_PODCOEF_OPENERROR;
  }

  int endian_check = 1;
  int data_type    = 2;

  ofs1.write((char*)&endian_check, sizeof(int) );       // エンディアンチェック
  ofs1.write((char*)&data_type, sizeof(int) );          // データタイプ
  ofs1.write((char*)&numStep, sizeof(int) );            // タイムステップ数
  ofs1.write((char*)&numCoef, sizeof(int) );            // 係数出力数（Layer1以降）
  ofs1.write((char*)&numCalculatedLayer, sizeof(int) ); // 圧縮時計算レイヤー数

  // Layer0
  ofs1.write((char*)&pCoef_a_layer[0], sizeof(double)*numStep);  // 係数データ
  // Layer1以降
  for(int i=1; i<numCalculatedLayer; i++ ) {
    ofs1.write((char*)&pCoef_a_layer[numStep*i], sizeof(double)*numCoef);  // 係数データ
  }

  if( ofs1.bad() ) {
    return PFC::E_PFC_ERROR_WRITE_PODCOEF_DATA;
  }

  ofs1.close();

  return PFC::E_PFC_SUCCESS;
}



// #################################################################
// 係数ファイル入力（読み込み）処理
PFC::E_PFC_ERRORCODE
CPfcPodFile::ReadCoefFile(
                const std::string&  dirPath,            // (in)  入力ディレクトリ
                const std::string&  prefix,             // (in)  属性名
                const int           regionID,           // (in)  領域ID  (<0: 領域分割なし)
                const bool          bSingle,            // (in) 単一領域フラグ
                int&                numStep,            // (out) タイムステップ数
                                                        //          == Layer0の係数出力数
                int&                numCoef,            // (out) 係数出力数（Layer1以降）
                int&                numCalculatedLayer, // (out) 圧縮時計算レイヤー数
                double*             &pCoef_a_layer,     // (out) 係数データ
                                                        //         [numCalculatedLayer][numStep];
                                                        //          ポインタの参照型であることに注意
                                                        //          使用後, delete [] pPod_base
                double**            &pIndex             // (out) 各レイヤーの係数データへのポインタ
                                                        //          [numCalculatedLayer]
                                                        //          ダブルポインタの参照型であることに注意
                                                        //          使用後, delete [] pIndex 必要

            )
{
  pCoef_a_layer = NULL;
  pIndex        = NULL;

  // ファイル名の生成
  char file_name[512];
  std::string fileName;

  if( bSingle ) {
    sprintf( file_name,"%s.coef",prefix.c_str() );
  } else {
    sprintf( file_name,"%s_id%06d.coef",prefix.c_str(),regionID );
  }
  std::string sFileName( file_name );

  std::string sFilePath = PFC::PfcPath_ConnectPath( dirPath, sFileName);

  ifstream ifs1(sFilePath.c_str(), ios::in | ios::binary);
  if (!ifs1) { 
    PFC_PRINT( "### ERROR ### Can't open filename=%s\n",sFilePath.c_str() );
    return PFC::E_PFC_ERROR_READ_PODCOEF_OPENERROR;
  }

  int  endian_check_compare = 1;
  int  endian_check;
  bool endian_chg;
  int  data_type;

  ifs1.read((char*)&endian_check, sizeof(int) );        // エンディアンチェック
  if( endian_check == endian_check_compare ) {
    endian_chg = false;
  } else {
    endian_chg = true;
  }

  ifs1.read((char*)&data_type, sizeof(int) );       // データタイプ
  if( endian_chg ) endswap(&data_type);

  ifs1.read((char*)&numStep, sizeof(int) );             // タイムステップ数
  if( endian_chg ) endswap(&numStep);

  ifs1.read((char*)&numCoef, sizeof(int) );             // 係数出力数（Layer1以降）
  if( endian_chg ) endswap(&numCoef);

  ifs1.read((char*)&numCalculatedLayer, sizeof(int) ); // 圧縮時計算レイヤー数
  if( endian_chg ) endswap(&numCalculatedLayer);

  pCoef_a_layer = new double[numStep*numCalculatedLayer];
  // Layer0
  ifs1.read((char*)&pCoef_a_layer[0], sizeof(double)*numStep);  // 係数データ
  // Layer1以降
  for(int i=1; i<numCalculatedLayer; i++ ) {
    ifs1.read((char*)&pCoef_a_layer[numStep*i], sizeof(double)*numCoef);  // 係数データ
    for(int j=numCoef; j<numStep; j++ ) {
      pCoef_a_layer[numStep*i+j] = 0.0;
    }
  }
  
  if( endian_chg ) {
    for(int i=0; i<numStep*numCalculatedLayer; i++ ) {
      endswap(&pCoef_a_layer[i]);
    }
  }

  // Index作成
  pIndex = new double*[numCalculatedLayer];
  for(int i=0; i<numCalculatedLayer; i++ ) {
    pIndex[i] = (pCoef_a_layer+numStep*i);
  }

  if( ifs1.bad() ) {
    return PFC::E_PFC_ERROR_READ_PODCOEF_DATA;
  }

  ifs1.close();

  return PFC::E_PFC_SUCCESS;
}


