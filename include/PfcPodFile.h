#ifndef _PFC_POD_FILE_H_
#define _PFC_POD_FILE_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcPodFile.h
 * @brief  CPfcPodFile Class Header  基底ファイル、係数ファイル用
 * @author aics
 */

#include "PfcDefine.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>
//#include <errno.h>
//#include <sys/stat.h>
//#include <typeinfo>
//#include <set>
//#include <map>
#include <string>

using namespace std;


/** PFC POD File class */
class CPfcPodFile {

public:

  /**
   * @brief 基底ファイル出力処理
   * @param [in]  outDirPath            出力ディレクトリ
   * @param [in]  prefix                属性名
   * @param [in]  regionID              領域ID
   * @param [in]  bSingle               単一領域フラグ
   * @param [in]  numStep               タイムステップ数
   * @param [in]  numParallel           圧縮時並列数
   * @param [in]  numCalculatedLayer    圧縮時計算レイヤー数
   * @param [in]  nSizes                各ランク内の要素数
   * @param [in]  pPod_base             基底データ
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  WriteBaseFile(
                const std::string&  outDirPath,         // (in) 出力ディレクトリ
                const std::string&  prefix,             // (in) 属性名
                const int           regionID,           // (in) 領域ID
                const bool          bSingle,            // (in) 単一領域フラグ
                const int           numStep,            // (in) タイムステップ数
                const int           numParallel,        // (in) 圧縮時並列数
                const int           numCalculatedLayer, // (in) 圧縮時計算レイヤー数
                const int*          nSizes,             // (in) 各ランク内の要素数
                double*             pPod_base           // (in) 基底データ
            );

  /**
   * @brief 基底ファイル出力処理（デバッグ用）
   * @detail   途中のレイヤーの基底ファイルを出力する
   * @param [in]  outDirPath            出力ディレクトリ
   * @param [in]  prefix                属性名
   * @param [in]  regionID              領域ID
   * @param [in]  layerNo               レイヤーNo （0起点）
   * @param [in]  numStep               タイムステップ数
   * @param [in]  numParallel           圧縮時並列数
   * @param [in]  numCalculatedLayer    圧縮時計算レイヤー数
   * @param [in]  nSize                 各ランクの要素数
   * @param [in]  pPod_base             各ランクの基底データ
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  WriteBaseFile_Debug(
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
            );


  /**
   * @brief 基底ファイル入力（読み込み）処理
   * @detail  open/read/close
   * @param [in]  dirPath               入力ディレクトリ
   * @param [in]  prefix                属性名
   * @param [in]  regionID              領域ID
   * @param [in]  bSingle               単一領域フラグ
   * @param [out  numStep               タイムステップ数
   * @param [out] numParallel           圧縮時並列数
   * @param [out] numCalculatedLayer    圧縮時計算レイヤー数
   * @param [out] pSizes                各ランク内の要素数
   *                                        使用後, delete [] pSizes 要
   * @param [out] pPod_base             基底データ
   *                                        使用後, delete [] pPod_base 要
   * @param [out] pIndex                各ランク内の基底データへのポインタ
   *                                        使用後, delete [] pIndex 要
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  ReadBaseFile(
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
            );


  /**
   * @brief 基底ファイルOpen処理(read mode)
   * @param [in]  dirPath               入力ディレクトリ
   * @param [in]  prefix                属性名
   * @param [in]  regionID              領域ID
   * @param [in]  bSingle               単一領域フラグ
   * @param [out] ifs                   入力ファイルストリーム
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  OpenBaseFile(
                const std::string&  dirPath,            // (in)  入力ディレクトリ
                const std::string&  prefix,             // (in)  属性名
                const int           regionID,           // (in)  領域ID
                const bool          bSingle,            // (in) 単一領域フラグ
                ifstream&           ifs                 // (out) 入力ファイルストリーム
            );


  /**
   * @brief 基底ファイルClose処理
   * @param [in]  ifs       入力ファイルストリーム
   * @return なし
   */
  static void
  CloseBaseFile(
                ifstream&   ifs
            );


  /**
   * @brief 基底ファイルヘッダ部読み込み処理
   * @param [in]  ifs                 入力ファイルストリーム
   * @param [out] numStep,            タイムステップ数
   * @param [out] numParallel,        圧縮時並列数
   * @param [out] numCalculatedLayer  圧縮時計算レイヤー数
   * @param [out] pSizes              各ランク内の要素数
   *                                    ポインタの参照型であることに注意
   *                                    使用後, delete [] pSizes 必要
   * @param [out] total_header_bsize  ヘッダ部のトータルサイズ(byte)
   * @param [out] total_base_size     基底データのトータルサイズ
   * @param [out] endian_chg          エンディアン変換フラグ
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  ReadBaseFileHeader(
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
            );


  /**
   * @brief 基底データ（１成分）読み込み処理
   * @param [in]  ifs                 入力ファイルストリーム
   * @param [in]  endian_chg          エンディアン変換フラグ
   * @param [in]  total_header_bsize  ヘッダ部のトータルサイズ(byte)
   * @param [in]  index,              基底データ部の読み込みインデックス
   * @param [out] v                   １成分データ
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  ReadBaseFile1Elm(
                ifstream&   ifs,                // (in)  入力ファイルストリーム
                bool        endian_chg,         // (in)  エンディアン変換フラグ
                int         total_header_bsize, // (in)  ヘッダ部のトータルサイズ(byte)
                int         index,              // (in)  基底データ部の読み込みインデックス
                double&     v                   // (out) 基底データ(１成分)
            );

  /**
   * @brief 基底ファイル入力（読み込み）処理（デバッグ用）
   * @detail  デバッグ用の基底ファイルを読み込む
   * @param [in]  dirPath               入力ディレクトリ
   * @param [in]  prefix                属性名
   * @param [in]  regionID              領域ID
   * @param [in]  layerNo               レイヤーNo （0起点)
   * @param [out  numStep               タイムステップ数
   * @param [out] numParallel           圧縮時並列数
   * @param [out] numCalculatedLayer    圧縮時計算レイヤー数
   * @param [out] nSize                 各ランクの要素数
   * @param [out] pPod_base             基底データ
   *                                        使用後, delete [] pPod_base 要
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  ReadBaseFile_Debug(
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
            );

  /**
   * @brief 係数ファイル出力処理
   * @param [in]  outDirPath            出力ディレクトリ
   * @param [in]  prefix                属性名
   * @param [in]  regionID              領域ID
   * @param [in]  bSingle               単一領域フラグ
   * @param [in]  numStep               タイムステップ数
   * @param [in]  numCoef               係数出力数（Layer1以降）
   * @param [in]  numCalculatedLayer    圧縮時計算レイヤー数
   * @param [in]  pCoef_a_layer         係数データ
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  WriteCoefFile(
                const std::string&  outDirPath,         // (in) 出力ディレクトリ
                const std::string&  prefix,             // (in) 属性名
                const int           regionID,           // (in) 領域ID 
                const bool          bSingle,             // (in) 単一領域フラグ
                const int           numStep,            // (in) タイムステップ数
                                                        //          == Layer0の係数出力数
                const int           numCoef,            // (in) 係数出力数（Layer1以降）
                const int           numCalculatedLayer, // (in) 圧縮時計算レイヤー数
                double*             pCoef_a_layer       // (in) 係数データ
            );

  /**
   * @brief 係数ファイル入力（読み込み）処理
   * @param [in]  dirPath               入力ディレクトリ
   * @param [in]  prefix                属性名
   * @param [in]  regionID              領域ID
   * @param [out] numStep               タイムステップ数（Layer0係数出力数）
   * @param [out] numCoef               係数出力数（Layer1以降）
   * @param [out] numCalculatedLayer    圧縮時計算レイヤー数
   * @param [out] pCoef_a_layer         係数データ
   *                                        使用後, delete [] pCoef_a_layer 要
   * @param [out] pIndex                各ランク内の係数データへのポインタ
   *                                        使用後, delete [] pIndex 要
   * @return 終了コード
   */
  static PFC::E_PFC_ERRORCODE
  ReadCoefFile(
                const std::string&  dirPath,            // (in)  入力ディレクトリ
                const std::string&  prefix,             // (in)  属性名
                const int           regionID,           // (in)  領域ID
                const bool          bSingle,             // (in) 単一領域フラグ
                int&                numStep,            // (out) タイムステップ数
                                                        //          == Layer0の係数出力数
                int&                numCoef,            // (out) 係数出力数（Layer1以降）
                int&                numCalculatedLayer, // (out) 圧縮時計算レイヤー数
                double*             &pCoef_a_layer,     // (out) 係数データ
                                                        //         [numCalculatedLayer][numStep];
                                                        //          ポインタの参照型であることに注意
                                                        //          使用後, delete [] pPod_base 要
                double**            &pIndex             // (out) 各レイヤーの係数データへのポインタ
                                                        //          [numCalculatedLayer]
                                                        //          ポインタの参照型であることに注意
                                                        //          使用後, delete [] pIndex 要
            );
                        
protected :


};

#endif // _PFC_POD_FILE_H_
