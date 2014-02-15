#ifndef _PFC_COMPRESS_H_
#define _PFC_COMPRESS_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompress.h
 * @brief  CPfcCompress Class Header
 * @author aics
 */

#include "PfcDefine.h"
#ifdef USE_PMLIB
#include "PfcPerfMon.h"
#endif
#include "cio_DFI.h"
#include "PfcDivision.h"
#include "PfcCompressCioPod.h"
#include "PfcFunction.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <typeinfo>
#include <string>
#include <vector>


/** PFC Compress  main class */
class CPfcCompress {

public:

protected :
  // POD用圧縮本体
  CPfcCompressCioPod* m_pCioPod;        // POD圧縮用クラス
                                        //  入力：CIOファイル
  
  // 初期化時の指定条件
  MPI_Comm      m_comm;                 ///< MPI コミュニケータ
  std::string   m_dfiFilePath;          ///<CIOのDFIファイル
  std::string   m_outDirPath;           ///<フィールドデータの出力ディレクトリパス
  std::string   m_compressFormat;       // 圧縮形式
  double        m_compressError;        // 誤差率(%)  ユーザ指定誤差
  int           m_domainDivision[3];    ///<計算領域の分割数
  int           m_startStep;            ///<開始タイムステップ
                                        //    == -1 dfiファイルに含まれる全タイムステップ
  int           m_endStep;              ///<終了タイムステップ
                                        //    == -1 dfiファイルに含まれる全タイムステップ
  int           m_optFlags;             // オプション フラグ  （マージした値）
                                        //  == 0 オプションなし

  // DFI情報
  cio_DFI*      m_pDfiIN;               // 入力となるDFI情報
  
  // 属性情報 
  //    取扱いが容易な様にDFI情報から抽出,endianは圧縮時のマシンのendian
  std::string           m_prefix;       // ベースファイル名  （属性名）
  std::string           m_fileFormat;   // ファイルタイプ、拡張子
  PFC::E_PFC_DTYPE      m_dataType;     // 配列のデータタイプ （DFIの情報）
                                        //    DFIのデータタイプ E_PFC_FLOAT32/E_PFC_FLOAT64
                                        //    PFC出力はE_PFC_FLOAT64のみ
  PFC::E_PFC_ENDIANTYPE m_endianType;   // 出力時のEndian形式  E_PFC_LITTLE / E_PFC_BIG 
  PFC::E_PFC_ARRAYSHAPE m_arrayShape;   // 配列形状    E_PFC_IJKN / E_PFC_NIJK
  int                   m_numComponent; // 成分数

  
  
  // 全体としての情報
  int           m_numRank;              // 全体のランク数( m_comm内 )
                                        //    = m_numRegion*m_numParallel
  int           m_numRegion;            // 全体の分割領域数
                                        //   = m_domainDivision[0]*m_domainDivision[1]*m_domainDivision[2]
  int           m_numParallel;          // 時間軸方向の並列数（分割領域内のランク数）
                                        //   PODではm_numTimeStepから求まる値
                                        
  int           m_numStep;              // 対象とするタイムステップ数
  vector<int>   m_stepList;             // タイムステップリスト（対象となるタイムステップ）
                                        //   0からの連番とは限らないことに注意


  // 分割領域毎の情報
  int           m_regionID;             // 分割領域のID : 0～(m_numTotalRegion-1)
  int           m_regionHead[3];        // 分割計算領域の開始位置
  int           m_regionTail[3];        // 分割計算領域の終了位置
  int           m_regionSize;           // 分割領域のサイズ
                                        //     = m_numComponent*
                                        //        m_regionVoxelSize[0]*m_regionVoxelSize[1]*m_regionVoxelSize[2]

  int           m_regionMasterRankID;   // 分割領域内のマスターランクID
                                        //   分割領域内のランクIDの先頭
                                        //   分割領域内の情報はマスターランクIDに集めて出力する

  
  // ランク毎の情報
  int           m_myRankID;             // 自身のランク番号 : 0～(m_numRank-1)
  int           m_myIDinRegion;         // 自身の領域内のID : 0～(m_numParallel-1)


public:
  /** コンストラクタ */
  CPfcCompress();
  
  /**　デストラクタ */
  ~CPfcCompress();

  /**
   * @brief 圧縮クラス 初期化
   * @param [in]  comm            MPIコミュニケータ
   * @param [in]  dfiFilePath     DFIファイルパス
   * @param [in]  outDirPath      フィールドデータ出力ディレクトリパス
   * @param [in]  compressFormat  圧縮フォーマット
   *                                   "pod"   POD圧縮
   * @param [in]  compressError   圧縮率
   * @param [in]  domainDivision  領域の分割数
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  Init(
           const MPI_Comm     comm, 
           const std::string& dfiFilePath,
           const std::string& outDirPath,
           const std::string& compressFormat,
           const double       compressError,
           const int          domainDivision[3]
       );

  /**
   * @brief 圧縮クラス 初期化
   * @param [in]  comm            MPIコミュニケータ
   * @param [in]  dfiFilePath     DFIファイルパス
   * @param [in]  outDirPath      フィールドデータ出力ディレクトリパス
   * @param [in]  compressFormat  圧縮フォーマット
   *                                   "pod"   POD圧縮
   * @param [in]  compressError   誤差率(%)
   * @param [in]  domainDivision  領域の分割数
   * @param [in]  startStep       開始ステップ  -1 全タイムステップ
   * @param [in]  endStep         終了ステップ  -1 全タイムステップ
   * @param [in]  optFlags        オプション    0  オプションなし
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  Init(
           const MPI_Comm     comm, 
           const std::string& dfiFilePath,
           const std::string& outDirPath,
           const std::string& compressFormat,
           const double       compressError,
           const int          domainDivision[3],
           const int          startStep,
           const int          endStep,
           const int          optFlags
       );

  /**
   * @brief 圧縮＆圧縮ファイル出力
   * @param [out] ret     終了コード
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  WriteData( void );

  /**
   * @brief proc.pfcファイル出力
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  WriteProcFile( void );
  
  // 出力処理

  /**
   * @brief index.pfcファイル出力
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  WriteIndexPfcFile( void );

protected :

 /**
   * @brief proc.pfc Division&Region情報の収集＆作成
   * @param [out]  division    proc.pfc Division&Regionクラス
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  GatherAndCreateDivisionInfo(
                         CPfcDivision& division
                    );
                                        

};
#endif // _PFC_COMPRESS_H_
