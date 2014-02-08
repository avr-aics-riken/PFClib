/*
 * frm (File Rank Mapper)
 *
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/**
 * @file Staging_Define.h
 * Staging マクロ記述ヘッダーファイル
 * @author aics
 */

#ifndef _STAGING_DEFINE_H
#define _STAGING_DEFINE_H

#include "frm_Version.h"

//##include "mpi.h"

/** 3次元インデクス(i,j,k) -> 1次元インデクス変換マクロ
 * @param[in] _I  i方向インデクス
 * @param[in] _J  j方向インデクス
 * @param[in] _K  k方向インデクス
 * @param[in] _NI i方向インデクスサイズ
 * @param[in] _NJ j方向インデクスサイズ
 * @param[in] _NK k方向インデクスサイズ
 * @param[in] _VC 仮想セル数
 * @return 1次元インデクス
 */
#define _IDX_S3D(_I,_J,_K,_NI,_NJ,_NK,_VC) \
( (long long)(_K+_VC) * (long long)(_NI+2*_VC) * (long long)(_NJ+2*_VC) \
+ (long long)(_J+_VC) * (long long)(_NI+2*_VC) \
+ (long long)(_I+_VC) \
)

/** Stagingのエラーコード **/
enum stg_ErrorCode
{
  STG_SUCCESS                      = 0    ///<正常終了
, STG_ERROR                        = 1000 ///< その他のエラー
, STG_ERROR_MISMATCH_NP_SUBDOMAIN  = 3003 ///< 並列数とサブドメイン数が一致していない
, STG_ERROR_OPEN_SBDM              = 3012 ///< ActiveSubdomainファイルのオープンに失敗
, STG_ERROR_READ_SBDM_HEADER       = 3013 ///< ActiveSubdomainファイルのヘッダー読み込みに失敗
, STG_ERROR_READ_SBDM_FORMAT       = 3014 ///< ActiveSubdomainファイルのフォーマットエラー
, STG_ERROR_READ_SBDM_DIV          = 3015 ///< ActiveSubdomainファイルの領域分割数読み込みに失敗
, STG_ERROR_READ_SBDM_CONTENTS     = 3016 ///< ActiveSubdomainファイルのContents読み込みに失敗
, STG_ERROR_SBDM_NUMDOMAIN_ZERO    = 3017 ///< ActiveSubdomainファイルの活性ドメイン数が0
, STG_ERROR_MISMATCH_DIV_SUBDOMAIN = 3018 ///< 領域分割数がActiveSubdomainファイルと一致していない
, STG_ERROR_INVALID_DIVNUM         = 3011 ///< 領域分割数が不正
};

/** Endian check */
enum stg_EMatchType
{
  STG_UnKnown = 0       ///<未定
, STG_Match   = 1       ///<一致
, STG_UnMatch = 2       ///<不一致
};

/** 粗密判定コード */
enum stg_EGlobalVoxel
{
  STG_E_GV_SAME
, STG_E_GVX2_SAME
, STG_E_OTHER
};

typedef std::map<int,int> headT;

#endif //_STAGING_DEFINE_H
