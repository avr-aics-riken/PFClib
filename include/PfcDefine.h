#ifndef _PFC_DEFINE_H_
#define _PFC_DEFINE_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/**
 * @file   PfcDefine.h
 * @brief  PFCの定義マクロ記述ヘッダーファイル
 * @author aics
 */

#ifdef _PFC_WITHOUT_MPI_
 #include "mpi_stubs.h"
#else
 #include "mpi.h"
#endif

// for Debug print
#if 0
#define PFC_PRINT  printf
#else
#define PFC_PRINT  PFC_PRINT_AT(__FILE__,__LINE__);printf
#endif

#define PFC_PRINT_AT(file,lineno)  \
    {  \
      printf ( "[%s:%4d] ",file,lineno );  \
    }

#ifdef DEBUG_PFC
#define DEBUG_PRINT  DEBUG_PRINT_AT(__FILE__,__LINE__);printf
#else
#define DEBUG_PRINT  if(0) printf
#endif

#define DEBUG_PRINT_AT(file,lineno)  \
    {  \
      printf ( "DBG [%s:%4d] ",file,lineno );  \
    }

//#define D_PFC_ON  "on"
//#define D_PFC_OFF "off"

// 浮動小数点誤差 epsilon
#define D_PFC_EPSILON  (1.0e-9)

// allowance of error percentage (%)
#define D_PFC_COMPRESS_ERROR_DEFAULT  (0.01)

//#define D_PFC_INT8    "Int8"
//#define D_PFC_INT16   "Int16"
//#define D_PFC_INT32   "Int32"
//#define D_PFC_INT64   "Int64"
//#define D_PFC_UINT8   "UInt8"
//#define D_PFC_UINT16  "UInt16"
//#define D_PFC_UINT32  "UInt32"
//#define D_PFC_UINT64  "UInt64"
#define D_PFC_FLOAT32 "Float32"
#define D_PFC_FLOAT64 "Float64"

#define D_PFC_IJNK "ijkn"
#define D_PFC_NIJK "nijk"

#define D_PFC_LITTLE "little"
#define D_PFC_BIG    "big"

#define _PFC_TAB_STR "  "

// オプション
        // 基底データレイヤー毎出力（デバッグ用）
#define D_PFC_OPT_FLAG_SAVE (0x00000001)

#define PFC_CHECK_OPT_SAVE(opt_flags)  (opt_flags&D_PFC_OPT_FLAG_SAVE)

/** namespace の設定 */
namespace PFC
{

#if 0
/** スイッチ　on or off */
  enum E_PFC_ONOFF
  {
    E_PFC_OFF = 0,           ///< off
    E_PFC_ON                 ///< on
  };
#endif

/** 圧縮形式 */
  enum E_PFC_COMPRESS_FORMAT
  {
    E_PFC_COMPRESS_FMT_UNKNOWN = -1,  ///< 未定
    E_PFC_COMPRESS_FMT_POD = 1        ///< POD format
  };

/** データ形式 */
  enum E_PFC_DTYPE
  {
    // 実際使われるのはE_PFC_FLOAT64のみ
    E_PFC_DTYPE_UNKNOWN = 0, ///< 未定
    E_PFC_INT8,              ///< char
    E_PFC_INT16,             ///< short
    E_PFC_INT32,             ///< int
    E_PFC_INT64,             ///< long long
    E_PFC_UINT8,             ///< unsigned char
    E_PFC_UINT16,            ///< unsigned short
    E_PFC_UINT32,            ///< unsigned int
    E_PFC_UINT64,            ///< unsigned long long
    E_PFC_FLOAT32,           ///< float
    E_PFC_FLOAT64            ///< double
  };

/** 配列形式 */
  enum E_PFC_ARRAYSHAPE
  {
    E_PFC_ARRAYSHAPE_UNKNOWN=-1, ///< 未定
    E_PFC_IJKN=0,                ///< ijkn
    E_PFC_NIJK                   ///< nijk
  };

/** Endian形式 */
  enum E_PFC_ENDIANTYPE
  {
    E_PFC_ENDIANTYPE_UNKNOWN=-1,
    E_PFC_LITTLE=0,
    E_PFC_BIG
  };

#if 0
/** 読込みタイプコード */
  enum E_PFC_READTYPE
  {
    E_PFC_SAMEDIV_SAMERES=1,     ///<同一分割＆同一密度
    E_PFC_SAMEDIV_REFINEMENT,    ///<同一分割＆粗密
    E_PFC_DIFFDIV_SAMERES,       ///<MxN＆同一密度
    E_PFC_DIFFDIV_REFINEMENT,    ///<MxN＆粗密
    E_PFC_READTYPE_UNKNOWN,      ///<error
  };
#endif

/** PFCのエラーコード */
  enum E_PFC_ERRORCODE
  {
    E_PFC_SUCCESS                           = 1,    ///< 正常終了
    E_PFC_ERROR                             = -1,   ///< エラー終了

    // READ系 (pfc_cntl)
    E_PFC_ERROR_READ_CNTLFILE_OPENERROR     = 500,
    E_PFC_ERROR_READ_CNTL_DOMAINDIVISION    = 501,  // PfcCompressCntl/DomainDivision
    E_PFC_ERROR_READ_CNTL_NO_ITEM           = 510,
    E_PFC_ERROR_READ_CNTL_ITEMCNTL          = 511,  // PfcCompressCntl/ItemCntl
    E_PFC_ERROR_READ_CNTL_DFI_PATH          = 512,  // PfcCompressCntl/ItemCntl/InputDfiPath
    E_PFC_ERROR_READ_CNTL_OUTDIR_PATH       = 513,  // PfcCompressCntl/ItemCntl/OutputDirectoryPath
    E_PFC_ERROR_READ_CNTL_COMPRESS_FMT      = 514,  // PfcCompressCntl/ItemCntl/CompressFormat
    E_PFC_ERROR_READ_CNTL_PROCFILE_SAVE     = 515,  // PfcCompressCntl/ItemCntl/ProcFileSave

    // READ系 (index.pfc)
    E_PFC_ERROR_READ_INDEXFILE_OPENERROR    = 1000,
    E_PFC_ERROR_READ_FILEINFO               = 1010, // FileInfo
    E_PFC_ERROR_READ_PFC_DIRECTORYPATH      = 1011, // FileInfo/DirectoryPath 読込みエラー
    E_PFC_ERROR_READ_PFC_PREFIX             = 1012, // FileInfo/Prefix
    E_PFC_ERROR_READ_PFC_FILEFORMAT         = 1013, // FileInfo/FileFormat
    E_PFC_ERROR_READ_PFC_GUIDECELL          = 1014, // FileInfo/GuideCell
    E_PFC_ERROR_READ_PFC_DATATYPE           = 1015, // FileInfo/DataType
    E_PFC_ERROR_READ_PFC_ENDIAN             = 1016, // FileInfo/Endian
    E_PFC_ERROR_READ_PFC_ARRAYSHAPE         = 1017, // FileInfo/ArrayShape
    E_PFC_ERROR_READ_PFC_COMPONENT          = 1018, // FileInfo/Component
    E_PFC_ERROR_READ_COMPRESSINFO           = 1030, // CompressInfo
    E_PFC_ERROR_READ_PFC_COMPRESSFORMAT     = 1031, // CompressInfo/CompressFormat
    E_PFC_ERROR_READ_PFC_COMPRESSERROR      = 1032, // CompressInfo/CompressError
    E_PFC_ERROR_READ_PFC_CALCULATEDLAYER    = 1033, // CompressInfo/CalculatedLayer
    E_PFC_ERROR_READ_PFC_VERSION            = 1034, // CompressInfo/Version
    E_PFC_ERROR_READ_PFC_STARTSTEP          = 1035, // CompressInfo/StartStep
    E_PFC_ERROR_READ_PFC_ENDSTEP            = 1036, // CompressInfo/EndStep
    E_PFC_ERROR_READ_FILEPATH               = 1040, // FilePath
    E_PFC_ERROR_READ_PFC_DFIPATH            = 1041, // FilePath/DfiPath
    E_PFC_ERROR_READ_PFC_PFCPROCESS         = 1042, // FilePath/PfcProcess
    E_PFC_ERROR_READ_UNITLIST               = 1050, // UnitList
    E_PFC_ERROR_READ_TIMESLICE              = 1060, // TimeSlice

    // WRITE系 (index.pfc)
    E_PFC_ERROR_WRITE_INDEXFILENAME_EMPTY   = 1100,
    E_PFC_ERROR_WRITE_INDEXFILE_OPENERROR   = 1101,
    E_PFC_ERROR_WRITE_FILEINFO              = 1110, // FileInfo
    E_PFC_ERROR_WRITE_PFC_DATATYPE          = 1111, // FileInfo/DataType
    E_PFC_ERROR_WRITE_COMPRESSINFO          = 1130, // CompressInfo
    E_PFC_ERROR_WRITE_FILEPATH              = 1140, // FilePath
    E_PFC_ERROR_WRITE_UNITLIST              = 1150, // UnitList
    E_PFC_ERROR_WRITE_TIMESLICE             = 1160, // TimeSlice

    // READ系 (proc.pfc)
    E_PFC_ERROR_READ_PROCFILE_OPENERROR     = 1200,
    E_PFC_ERROR_READ_DOMAIN                 = 1210, // Domain
    E_PFC_ERROR_READ_PFC_GLOBALORIGIN       = 1211, // Domain/GlobalOrigin
    E_PFC_ERROR_READ_PFC_GLOBALREGION       = 1212, // Domain/GlobalRegion
    E_PFC_ERROR_READ_PFC_GLOBALVOXEL        = 1213, // Domain/GlobalVoxel
    E_PFC_ERROR_READ_PFC_GLOBALDIVISION     = 1214, // Domain/GlobalDivision
    E_PFC_ERROR_READ_DIVISION               = 1220, // Division
    E_PFC_ERROR_READ_PFC_NO_REGION          = 1221, // Division/Region 読込みエラー
    E_PFC_ERROR_READ_PFC_REGION_ID          = 1222, // Division/Region/ID
    E_PFC_ERROR_READ_PFC_REGION_VOXELSIZE   = 1223, // Division/Region/VoxelSize
    E_PFC_ERROR_READ_PFC_REGION_HEADINDEX   = 1224, // Division/Region/HeadIndex
    E_PFC_ERROR_READ_PFC_REGION_TAILINDEX   = 1225, // Division/Region/TailIndex

    // WRITE系 (proc.pfc)
    E_PFC_ERROR_WRITE_PROCFILE_OPENERROR    = 1300,
    E_PFC_ERROR_WRITE_DOMAIN                = 1310,
    E_PFC_ERROR_WRITE_DIVISION              = 1320,

    // READ系 (基底ファイル)
    E_PFC_ERROR_READ_PODBASE_OPENERROR      = 2000,
    E_PFC_ERROR_READ_PODBASE_HEADER         = 2005,
    E_PFC_ERROR_READ_PODBASE_DATA           = 2010,

    // WRITE系 (基底ファイル)
    E_PFC_ERROR_WRITE_PODBASE_OPENERROR     = 2100,
    E_PFC_ERROR_WRITE_PODBASE_DATA          = 2110,

    // READ系 (係数ファイル)
    E_PFC_ERROR_READ_PODCOEF_OPENERROR      = 2200,
    E_PFC_ERROR_READ_PODCOEF_DATA           = 2210,

    // WRITE系 (係数ファイル)
    E_PFC_ERROR_WRITE_PODCOEF_OPENERROR     = 2300,
    E_PFC_ERROR_WRITE_PODCOEF_DATA          = 2310,

    // その他のエラー
    E_PFC_ERROR_PFC_COMPRESSFORMAT          = 3000,
    E_PFC_ERROR_OUT_OF_RANGE                = 3100

  };

};

/** 3次元（スカラー）インデクス(i,j,k) -> 1次元インデクス変換マクロ
 *  @param[in] _I  i方向インデクス
 *  @param[in] _J  j方向インデクス
 *  @param[in] _K  k方向インデクス
 *  @param[in] _NI i方向インデクスサイズ
 *  @param[in] _NJ j方向インデクスサイズ
 *  @param[in] _NK k方向インデクスサイズ
 *  @param[in] _VC 仮想セル数
 *  @return 1次元インデクス
 */
#define _PFC_IDX_IJK(_I,_J,_K,_NI,_NJ,_NK,_VC) \
( (long long)((_K)+(_VC)) * (long long)((_NI)+2*(_VC)) * (long long)((_NJ)+2*(_VC)) \
+ (long long)((_J)+(_VC)) * (long long)((_NI)+2*(_VC)) \
+ (long long)((_I)+(_VC)) \
)

/** 2次元（スカラー）インデクス(i,j) -> 1次元インデクス変換マクロ
 *  @param[in] _I  i方向インデクス
 *  @param[in] _J  j方向インデクス
 *  @param[in] _NI i方向インデクスサイズ
 *  @param[in] _NJ j方向インデクスサイズ
 *  @param[in] _VC 仮想セル数
 *  @return 1次元インデクス
 */
#define _PFC_IDX_IJ(_I,_J,_NI,_NJ,_VC) \
( (long long)((_J)+(_VC)) * (long long)((_NI)+2*(_VC)) \
+ (long long)((_I)+(_VC)) \
)

/** 2次元（スカラー）インデクス(n,i,j) -> 1次元インデクス変換マクロ
 *  @param[in] _N  成分インデクス
 *  @param[in] _I  i方向インデクス
 *  @param[in] _J  j方向インデクス
 *  @param[in] _NI i方向インデクスサイズ
 *  @param[in] _NJ j方向インデクスサイズ
 *  @param[in] _NN 成分数
 *  @param[in] _VC 仮想セル数
 *  @return 1次元インデクス
 */
#define _PFC_IDX_NIJ(_N,_I,_J,_NI,_NJ,_NN,_VC) \
( (long long)(_NN)*_PFC_IDX_IJ(_I,_J,_NI,_NJ,_VC) \
 + (long long)(_N) \
)


/** 3次元（ベクトル）インデクス(i,j,k,n) -> 1次元インデクス変換マクロ
 *  @param[in] _I  i方向インデクス
 *  @param[in] _J  j方向インデクス
 *  @param[in] _K  k方向インデクス
 *  @param[in] _N  成分インデクス
 *  @param[in] _NI i方向インデクスサイズ
 *  @param[in] _NJ j方向インデクスサイズ
 *  @param[in] _NK k方向インデクスサイズ
 *  @param[in] _VC 仮想セル数
 *  @return 1次元インデクス
 */
#define _PFC_IDX_IJKN(_I,_J,_K,_N,_NI,_NJ,_NK,_VC) \
( (long long)(_N) * (long long)((_NI)+2*(_VC)) * (long long)((_NJ)+2*(_VC)) \
* (long long)((_NK)+2*(_VC)) \
+ _PFC_IDX_IJK(_I,_J,_K,_NI,_NJ,_NK,_VC) \
)

/** 3次元（ベクトル）インデクス(n,i,j,k) -> 1次元インデクス変換マクロ
 *  @param[in] _N  成分インデクス
 *  @param[in] _I  i方向インデクス
 *  @param[in] _J  j方向インデクス
 *  @param[in] _K  k方向インデクス
 *  @param[in] _NN 成分数
 *  @param[in] _NI i方向インデクスサイズ
 *  @param[in] _NJ j方向インデクスサイズ
 *  @param[in] _NK k方向インデクスサイズ
 *  @param[in] _VC 仮想セル数
 *  @return 1次元インデクス
 */
#define _PFC_IDX_NIJK(_N,_I,_J,_K,_NN,_NI,_NJ,_NK,_VC) \
( (long long)(_NN) * _PFC_IDX_IJK(_I,_J,_K,_NI,_NJ,_NK,_VC) \
+ (long long)(_N) )


/** PFCファイルのTab出力
 *  @param[in] _FP  ファイルポインタ
 *  @param[in] _NTAB インデント数
 */
#define _PFC_WRITE_TAB(_FP,_NTAB) {\
 for(int _NTCNT=0; _NTCNT<_NTAB; _NTCNT++) fprintf(_FP,_PFC_TAB_STR); \
}

#endif /* _PFC_DEFINE_H_ */
