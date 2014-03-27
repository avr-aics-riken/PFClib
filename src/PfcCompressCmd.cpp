/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   CPfcCompressCmd.cpp
 * @brief  CPfcCompressCmd Class
 * @author aics
 */

#include "mpi.h"
#include "PfcCompressCmd.h"
#include "PfcCompress.h"


// #################################################################
// コンストラクタ
CPfcCompressCmd::CPfcCompressCmd( const std::string& compressCntlPath )
{
  m_compressCntlPath = compressCntlPath;
  m_domainDivision[0] = 0;
  m_domainDivision[1] = 0;
  m_domainDivision[2] = 0;
}


// #################################################################
// デストラクタ
CPfcCompressCmd::~CPfcCompressCmd()
{

}


// #################################################################
// 圧縮実行
PFC::E_PFC_ERRORCODE
CPfcCompressCmd::Execute( void )
{
  PFC::E_PFC_ERRORCODE ret;

  ret = ReadCompressCntl();
  if( ret != PFC::E_PFC_SUCCESS ) {
    return ret;
  }
  
  // 制御データの数 ループ
  for( int i = 0; i<m_cmdList.size(); i++ )
  {
    CPfcCompress pfcCompress;   // 圧縮クラス
    
    // 圧縮クラス 初期化
    ret = pfcCompress.Init (
               MPI_COMM_WORLD,
               m_cmdList[i].m_dfiFilePath,
               m_cmdList[i].m_outDirPath,
               m_cmdList[i].m_compressFormat,
               m_cmdList[i].m_compressError,
               m_domainDivision,
               m_cmdList[i].m_startStep,
               m_cmdList[i].m_endStep,
               m_cmdList[i].m_optFlags
            );
    if( ret != PFC::E_PFC_SUCCESS ) {
       PFC_PRINT("Error at pfcCompress.Init() in CPfcCompressCmd::Execute()  i=%d\n",i);
      return ret;
    }
    
    // proc.pfcファイル出力
    if( m_cmdList[i].m_procFileSave == "ON" ) 
    {
      ret = pfcCompress.WriteProcFile();
      if( ret != PFC::E_PFC_SUCCESS ) {
        PFC_PRINT("Error at pfcCompress.WriteProcFile() in CPfcCompressCmd::Execute()  i=%d\n",i);
        return ret;
      }
    }
    
    // 圧縮＆圧縮データファイル出力処理
    ret = pfcCompress.WriteData();
    if( ret != PFC::E_PFC_SUCCESS ) {
      PFC_PRINT("Error at pfcCompress.Execute() in CPfcCompressCmd::Execute()  i=%d\n",i);
      return ret;
    }
    
  }
  
  ret = PFC::E_PFC_SUCCESS;

  return ret;
}

// #################################################################
// 圧縮制御情報ファイル読み込み
PFC::E_PFC_ERRORCODE
CPfcCompressCmd::ReadCompressCntl( void )
{
  PFC::E_PFC_ERRORCODE ret;
  CPfcTextParser tpCntl;
  std::string str;
  std::string label, label_base, label_leaf;
  int iv[3];
  
  CPfcCompressCmdElm cmdElm;

  /** pfc_cntl read */
  /** TPインスタンス */
  tpCntl.getTPinstance();

  FILE* fp = fopen(m_compressCntlPath.c_str(),"rb");
  if( fp == NULL ) {
    PFC_PRINT("Can't open file. (%s)\n",m_compressCntlPath.c_str());
    ret = PFC::E_PFC_ERROR_READ_CNTLFILE_OPENERROR;
    return ret;
  }
  fclose(fp);

  /** 入力ファイル pfc_cntl をセット */
  int ierror = 0;
  ierror = tpCntl.readTPfile(m_compressCntlPath);
  if ( ierror )
  {
    PFC_PRINT("\tinput file not found '%s'\n",m_compressCntlPath.c_str());
    ret = PFC::E_PFC_ERROR_READ_CNTLFILE_OPENERROR;
    return ret;
  }
  
  //DomainDivision
  label = "/PfcCompressCntl/DomainDivision";
  for (int n=0; n<3; n++) iv[n]=0;
  if ( !(tpCntl.GetVector(label, iv, 3 )) ) 
  {
    PFC_PRINT("\tPFC Parsing error : fail to get '%s'\n",label.c_str());
    return PFC::E_PFC_ERROR_READ_CNTL_DOMAINDIVISION;
  }
  m_domainDivision[0]=iv[0];
  m_domainDivision[1]=iv[1];
  m_domainDivision[2]=iv[2];

  //ItemCntl
  int nnode=0;
  label_base = "/PfcCompressCntl";
  if ( tpCntl.chkNode(label_base) )  //があれば
  {
    nnode = tpCntl.countLabels(label_base);
  }

  for (int i=0; i<nnode; i++) {

    //int ncnt=0;

    if(!tpCntl.GetNodeStr(label_base,i+1,&str))
    {
      PFC_PRINT("\tPFC Cntl Parsing error : No Elem name\n");
      return PFC::E_PFC_ERROR_READ_CNTL_NO_ITEM;
    }
    if( strcasecmp(str.substr(0,8).c_str(), "ItemCntl") ) continue;
    label_leaf=label_base+"/"+str;

    // 圧縮制御データ要素の読込み
    ret = cmdElm.ReadElm(tpCntl,label_leaf);

    if( ret == PFC::E_PFC_SUCCESS ) {
      m_cmdList.push_back(cmdElm); 
    } else {
      PFC_PRINT("\tItemCntl Data in pfc_cntl Read error %s\n", m_compressCntlPath.c_str());
      ret = PFC::E_PFC_ERROR_READ_CNTL_ITEMCNTL;
      return ret;
    }

  }

  /** TextParserの破棄 */
  tpCntl.remove();

  ret = PFC::E_PFC_SUCCESS;

  return ret;
}
