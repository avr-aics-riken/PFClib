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
 * @file Staging_Utility.h
 * @brief Staging Class Header
 * @author aics
 */

#ifndef _STAGING_UTILITY_H
#define _STAGING_UTILITY_H

#include "cio_DFI.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <float.h>
#include <set>
#include <map>

#include "ActiveSubDomain.h"
#include "Staging_Define.h"
#include "frm_EndianUtil.h"

/** Staging main class */
class Staging {

public:

  /** staging 用 info.txt ファイルの Process */
  struct Rank
  {
    int RankID;
    int VoxelSize[3];
    int HeadIndex[3];
    int TailIndex[3];
    int RankPosition[3];
  };

  /** コンストラクタ **/
  Staging(){
    //m_step=-1;
    m_startStep=-1;
    m_endStep=-1;
    m_inPath="";
    m_outPath="";
    m_cntlPath="";
  };

  /** デストラクタ **/
  ~Staging(){};

public:
  
  //cio_DFI CIO;
  cio_DFI *DFI;

  const cio_FileInfo  *dfi_Finfo;   ///< DFI cio_FileInfoのポインタ
  const cio_FilePath  *dfi_Fpath;   ///< DFI cio_FilePathのポインタ
  const cio_Unit      *dfi_Unit;    ///< DFI cio_Unitのポインタ
  const cio_Domain    *dfi_Domain;  ///< DFI cio_Domainのポインタ
  const cio_MPI       *dfi_MPI;     ///< DFI cio_MPIのポインタ
  const cio_TimeSlice *dfi_TSlice;  ///< DFI cio_TimeSliceのポインタ
  //const cio_Process   *dfi_Process; ///< DFI cio_Processのポインタ
        cio_Process   *dfi_Process; ///< DFI cio_Processのポインタ

  //int m_step;                 ///< Restart step番号
  int m_startStep;            ///< 開始step番号
  int m_endStep;              ///< 終了step番号
  string m_inPath;            ///< DFIディレクトリ
  string m_outPath;           ///< 出力ディレクトリ
  string m_cntlPath;          ///< 圧縮制御ファイル(pfc_cntl)パス
  string m_infofile;          ///< Staging用procファイル名
  int m_GVoxel[3];            ///< Staging用ボクセルサイズ
  int m_Gdiv[3];              ///< Staging用分割数（PFC領域分割数）
  string m_ActiveSubdomain;   ///< ActiveSubdomainファイル名
  int m_NumberOfRank;         ///< Staging用並列数（PFC領域数）
  vector<Rank> m_GRankInfo;   ///< Staging用並列情報テーブル 
  vector<string> m_dfi_fname; ///<STaging用DFIファイル名

  std::vector<ActiveSubDomain> m_subDomainInfo; ///< 活性サブドメイン情報

  
  int           m_numRankPfc;   // PFCの全体ランク数
                                //   PFC領域数*m_numParallel
  int           m_numStep;      // 対象とするタイムステップ数
  vector<int>   m_stepList;     // タイムステップリスト（対象となるタイムステップ）
                                //   0からの連番とは限らないことに注意
  int           m_numParallel;  // 時間軸方向の並列数（分割領域内のランク数）
                                //   PODではm_numTimeStepから求まる値
  vector<int>   m_startStepInID; // 領域内idの開始ステップ位置(0からの連番）
  vector<int>   m_numStepInID;   // 領域内idのステップ数
  vector<int>   m_idAtStep;      // ステップ毎の領域内id

  int *m_rankMap;
  int (*m_HeadTail)[6];
  int (*m_StartEnd)[6];

  std::set<int>m_headX;
  std::set<int>m_headY;
  std::set<int>m_headZ;

  headT m_mapX,m_mapY,m_mapZ;

public:

  /** 初期化、ファイルの読込み
   * @param[in] infofname ステージング用procファイル名 
   * @param[in] dfifname　dfiファイル名
   * @retval    true  ファイル読込み成功
   * @retval    false ファイル読込み失敗 
   */
  bool Initial(string infofname, string dfifname);

  /** ステージング用procファイルの読込み
   * @param[in] infofile ステージング用procファイル名
   * @retval   true     ファイル読込み成功 
   * @retval   false    ファイル読込み失敗
   */ 
  bool ReadInfo(string infofile);

  /** 領域分割数の取得
   * @return 領域分割数整数配列のポインタ
   */
  const int* GetDivNum() const;

  /** 活性サブドメイン情報の存在チェック
   * @param[in] subDomain チェックする活性サブドメイン情報
   * @retval    true      存在する
   * @retval    false     存在しない
   */ 
  bool IsExistSubdomain( ActiveSubDomain subDomain );

  /** 活性サブドメイン情報の追加
   * @param[in] subDomain 追加する活性サブドメイン情報
   * @retval    true      追加した
   * @retval    false     追加に失敗(同じ領域分割位置で追加済み)
   */
  bool AddSubdomain( ActiveSubDomain subDomain ); 

  /** 活性サブドメインの数を取得
   * 活性サブドメインの数＝活性サブドメイン情報配列のサイズだが、
   * この配列が空のとき、領域分割数でサブドメイン数を決定して返す
   * @return 活性サブドメインの数
   */
  int GetSubdomainNum() const; 

  /**  活性サブドメイン情報を取得
   * @param[in]  idx 登録順番号
   * @return 活性サブドメイン情報ポインタ
   */
  const ActiveSubDomain* GetSubdomainInfo( size_t idx ) const; 

  /** ランクマップを生成
   * @retval ランクマップ
   */ 
  //bool CreateRankMap();
  int* CreateRankMap();

  /** 有効なランクマップを生成
   * @retval 有効なランクマップ
   *
   */
  int* CreateActiveRankMap(); 

  /** ActiveSubdomainファイルのエンディアンチェック
   *  ActiveSubdomainファイルのエンディアンをチェック
   * @param[in]  ident               ActiveSubdomainファイルのIdentifier
   * @retval     STG_Match   一致
   * @retval     STG_UnMatch 不一致
   * @retval     STG_UnKnown フォーマットが異なる
   *
   */
  static stg_EMatchType isMatchEndianSbdmMagick( int ident ); 

  /** ActiveSubdomainファイルの読み込み
   * ActiveSubdomainファイルを読み込み、活性ドメイン情報を生成する
   * @param[in]  subDomainFile ActiveSubdomainファイル名
   * @return   終了コード(STG_SUCCESS=正常終了)
   */ 
   stg_ErrorCode ReadActiveSubdomainFile( std::string subDomainFile );

  /** ActiveSubdomainファイルの読み込み(static関数)
   * ActiveSubdomainファイルを読み込み、活性ドメイン情報を生成する
   * @param[in]  subDomainFile ActiveSubdomainファイル名
   * @param[out] subDomainInfo 活性ドメイン情報
   * @param[out] div           ActiveSubdiomainファイル中の領域分割数
   * @return   終了コード(STG_SUCCESS=正常終了) 
   */
   static stg_ErrorCode ReadActiveSubdomainFile( std::string subDomainFile,
                                                 std::vector<ActiveSubDomain>& subDomainInfo,
                                                 int div[3] );

  /** VOXEL数の取得
   * @return VOXEL数情報実数配列のポインタ
   */
   const int* GetVoxNum() const; 

  /** 領域情報のチェック
   * 活性サブドメイン配列が空のとき、全領域が活性サブドメインになるため
   * このチェック関数内で活性サブドメイン情報を生成する.
   * @param[in] nRank 並列プロセス数 
   * @return   終了コード(STG_SUCCESS=正常終了)
   */
   stg_ErrorCode CheckData( int nRank );

  /** head&tail情報の作成
   * @param[in] rankMap ランクマップ
   * @retval true  正常終了
   * @retval false エラー
   */
   bool CreateHeadTail(int* rankMap); 

  /** head&tail情報の作成
   * @param[in] rankMap ランクマップ
   * @param[in] Rankinfo ランク情報
   * @retval true  正常終了
   * @retval false エラー
   */
   bool CreateHeadTail(int* rankMap, vector<Rank>& RankInfo); 

  /** head&taileのセット
   */
   void SetHeadTail();

  /** Headマップの生成
   * @param[in] head head情報
   * @param[in] map headマップ
   */
   void CreateHeadMap(std::set<int>head, headT &map);
 
  /** 粗密データ判定
   * @param [in] Gvoxel     計算空間全体のボクセルサイズ（自）
   * @param [in] DFI_Gvoxel 計算空間全体のボクセルサイズ（DFI）
   * @return STG_E_GV_SAME:密 STG_E_GVX2_SAME:粗 STG_E_OTHER:その他
   */ 
  stg_EGlobalVoxel CheckGlobalVoxel(int Gvoxel[3], int DFI_Gvoxel[3]);


  /** 粗密用にhead,tailを作成
   * @param [out] HeadTail 粗密用heat,tail情報テーブル
   * @param [in]  numrank  dfiのランク数
   */
  void MakeGVX2HeadTeail(int (*HeadTail)[6], int numrank); 

  /** 検索範囲のしぼりこみ
   * @param [in] HeadTail heat,tail情報テーブル
   * @retval true  正常終了
   * @retval false エラー
   */
  bool CreateStartEnd(int (*HeadTail)[6]);

  /** ファイルをステージング用のディレクトリにコピー
   * @param [in] readRankList 読込みランクリスト
   * @retval true  正常終了
   * @retval false エラー
   */
  bool FileCopy(vector<int> readRankList, int myRank);

  /** dfi ファイル出力
   * @param [in] DFIファイル名
   * @retval true  正常終了
   * @retval false エラー
   */
  bool OutputDFI(string fname, int* rankMap);

  CIO::E_CIO_ERRORCODE
  WriteIndexDfiFile(const std::string dfi_name);
 
  /** ファイル名の作成
   * @param [in] RankID ランク番号
   * @param [in] step   読込みステップ番号
   * @param [in] mio    並列判定フラグ（逐次or並列の判定用）
   * @return 生成されたファイル名
   */
   std::string Generate_FileName(int RankID, int step, const bool mio);

  /** ディレクトリパスの作成
   * @param [in] path パス
   * @return error code
   */
   int MakeDirectory(string path);  

  /**
   * @brief ディレクトリパスの作成(MakeDirectory関数を呼出して作成)
   * @return error code　　　　　　　
   */
   int MakeDirectoryPath();

  /**
   * @brief ディレクトリパスの作成(system関数mkdirで作成)
   * @param
   * @return error code　　　　　　　
   */
  static int MakeDirectorySub( std::string path );

  /**
   * @brief dfiのパスとDirectoryPathを連結する関数
   * @return パス名
   */
  std::string Generate_Directory_Path();


};

#endif //_STAGING_UTILITY_H
