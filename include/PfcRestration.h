#ifndef _PFC_RESTRATION_H_
#define _PFC_RESTRATION_H_

/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcRestration.h
 * @brief  CPfcRestration Class Header
 * @author aics
 */

#include "PfcDefine.h"

#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <typeinfo>
#include <vector>
#include <string>

#include "PfcFileInfo.h"
#include "PfcCompressInfo.h"
#include "PfcFilePath.h"
#include "PfcUnitList.h"
#include "PfcTimeSlice.h"
#include "PfcDomain.h"
#include "PfcDivision.h"

#include "PfcRestrationRegion.h"    // 圧縮時の分割領域毎に管理しているクラス
                                    //    圧縮データはこれ毎に実体があるため


/** PFC restraion class */
class CPfcRestration {

public:

protected :
  std::string   m_pfcFilePath;  // index.pfcのファイルパス

  // index.pfc, proc.pfc関連
  CPfcFileInfo      m_fileInfo;         //  File情報（属性etc.）
  CPfcCompressInfo  m_compressInfo;     //  圧縮情報
  CPfcFilePath      m_filePath;         //  FilePathクラス
  CPfcUnitList      m_unitList;         //  単位系情報
  CPfcTimeSlice     m_timeSlice;        //  時系列データ情報
  int               m_numTimeStep;      //  タイムステップ数
  vector<int>       m_timeStepList;     //  タイムステップリスト
  
  // proc.pfc関連
  CPfcDomain        m_domain;       //  ドメイン情報
  CPfcDivision      m_division;     //  分割領域情報

  // ---- 以下は実行時の情報 ----------------------------
  bool          m_bLoadCompressData;    // 圧縮データのメモリロードフラグ

 // メモリ上にロードした領域
  int               m_head[3];      // 計算領域の開始位置
  int               m_tail[3];      // 計算領域の終了位置
  
  // 対象分割領域情報
  int                          m_numRegion;     // 対象分割領域数
  vector<int>                  m_regionIdList;  // 対象分割領域ID
                                                //  一部でも領域が重なるものを対象とする
  vector<CPfcRestrationRegion*> m_regionList;    // 対象分割領域


public:
  /** コンストラクタ */
  CPfcRestration();
  
  /**　デストラクタ */
  ~CPfcRestration();

  /**
   * @brief 展開クラス 初期化
   * @param [in]  pfcFilePath  index.pfcのファイルパス
   * @param [out] ret     終了コード
   * @return インスタンスされたクラスのポインタ
   */
  PFC::E_PFC_ERRORCODE
  Init(
        const std::string& pfcFilePath
      );

  /**
   * @brief 計算領域サイズ取得
   * @param [out] voxel 計算領域サイズ
   * @return 終了コード
   */
  void
  GetGlobalVoxel (
           int voxel[3]    // [out] 計算領域サイズ
       ); 


  /**
   * @brief 領域範囲(Head/Tail)計算
   * @param [in]  gDiv      計算空間の領域分割数
   * @param [in]  regionID  領域ID:  0～(gDiv[0]*gDiv[1]*gDiv[2]-1)
   * @param [out] head      計算領域の開始位置
   * @param [out] tail      計算領域の終了位置
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  GetHeadTail (
           const int gDiv[3], 
           const int regionID, 
           int head[3],
           int tail[3]
       ); 

  /**
   * @brief 圧縮データのメモリロード チェック
   *           タイムステップは全展開される前提
   * @param [in]  memUseMax  使用メモリMAX  単位(Mib)
   * @param [in]  head       計算領域の開始位置
   * @param [in]  tail       計算領域の終了位置
   * @param [out] loadRatio ロード可能な割合 ( 0.0 - 1.0 )
   * @return 終了コード PFC::E_PFC_SUCCESS ロード可
   *                    PFC::E_PFC_ERROR   ロード不可
   */
  PFC::E_PFC_ERRORCODE
  CheckCompressDataOnMem (
           const int memUseMax, 
           const int head[3],
           const int tail[3],
           double&   loadRatio
       ); 

  /**
   * @brief 圧縮データ ロード
   * @param [in]  head       計算領域の開始位置
   * @param [in]  tail       計算領域の終了位置
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  LoadCompressDataOnMem(
           const int head[3],
           const int tail[3]
       );


  /**
   * @brief ロードした圧縮データ削除
   * @return なし
   */
  void
  DeleteCompressDataOnMem( void );

  /**
   * @brief タイムステップリスト取得
   * @param [out] timeStepList  タイムステップリスト
   *                                index.pfcに記載されているStepの値（複数）を取得
   * @return 終了コード
   */
  PFC::E_PFC_ERRORCODE
  GetTimeStepList (
           vector<int>& timeStepList
       ); 

  /**
   * @brief データ読み込み（範囲指定）
   * @details  LoadCompressDataOnMem()が実行されている場合、メモリから展開する
   * @param [out] v         展開後の出力領域 
   *                              メモリは確保済みであること
   *                              head-tail間のボクセルサイズ*成分数
   * @param [in]  step      ステップ番号
   * @param [in]  head      計算領域の開始位置
   * @param [in]  tail      計算領域の終了位置
   * @return 終了コード
   *              E_PFC_SUCCESS      : OK
   *              E_PFC_OUT_OF_RANGE : LoadCompressDataOnMem()で設定された領域範囲外
   *              E_PFC_ERROR        : NG
   */
  PFC::E_PFC_ERRORCODE
  ReadData (
              double*  v,
              const int step, 
              const int head[3], 
              const int tail[3]
           );


  /**
   * @brief データ読み込み（位置指定）
   * @details  LoadCompressDataOnMem()が実行されている場合、メモリから展開する
   * @param [out] v         展開後の出力領域   (メモリは確保済みであること）
   * @param [in]  step      ステップ番号
   * @param [in]  ix        x position ( >= 1 ) headの開始は1からのため、それに合わせる
   * @param [in]  iy        y position ( >= 1 ) headの開始は1からのため、それに合わせる
   * @param [in]  iz        z position ( >= 1 ) headの開始は1からのため、それに合わせる
   * @return 終了コード
   *              E_PFC_SUCCESS      : OK（全タイムステップ可能）
   *              E_PFC_OUT_OF_RANGE : LoadCompressDataOnMem()で設定された領域範囲外
   *              E_PFC_ERROR        : NG
   */
  PFC::E_PFC_ERRORCODE
  ReadData (
              double*  v,
              const int step, 
              const int ix,
              const int iy, 
              const int iz
           );

};
#endif // _PFC_RESTRATION_H_
