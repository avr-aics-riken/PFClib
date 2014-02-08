/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCompress.cpp
 * @brief  CPfcCompress Class
 * @author aics
 */

#include "PfcCompress.h"
#include "PfcDomain.h"
#include "PfcDivision.h"


//#################################################################
// PFCファイル出力( proc.pfc )
PFC::E_PFC_ERRORCODE
CPfcCompress::WriteProcFile( void )
{
  // Division&Region情報の収集＆作成
  CPfcDivision pfcDivision;
  GatherAndCreateDivisionInfo( pfcDivision );
  

  // ランク０のみファイル出力
  if( m_myRankID != 0 ) return PFC::E_PFC_SUCCESS;

  // procファイル名の生成
  //std::string procFileName = CIO::cioPath_DirName(m_indexDfiName)+"/"+CIO::cioPath_FileName(DFI_Fpath.ProcDFIFile,".dfi");
  std::string filename = "proc.pfc";


  //proc.dfの出力
  FILE* fp = NULL;
#if 0
  // File exist ?
  bool flag = false;
  if ( fp = fopen(filename.c_str(), "r") )
  {
    flag = true;
    fclose(fp);
  }
#endif

  if( !(fp = fopen(filename.c_str(), "w")) )
  {
    PFC_PRINT("Can't open file.(%s)\n", filename.c_str());
    return PFC::E_PFC_ERROR_WRITE_PROCFILE_OPENERROR;
  }
#ifdef DEBUG_PFC
  else
  {
    DEBUG_PRINT("*** Open file.(%s)\n", filename.c_str());
  }
#endif

  //Domain {} の出力
  const cio_Domain* pCioDomain = m_pDfiIN->GetcioDomain(); 

  CPfcDomain pfcDomain (
             pCioDomain->GlobalOrigin, 
             pCioDomain->GlobalRegion, 
             pCioDomain->GlobalVoxel, 
             //pCioDomain->GlobalDivision
             m_domainDivision
            );
            
  if( pfcDomain.Write(fp, 0) != PFC::E_PFC_SUCCESS )
  {
    if (fp) fclose(fp);
    PFC_PRINT("error write domain.(%s)\n", filename.c_str());
    return PFC::E_PFC_ERROR_WRITE_DOMAIN;
  }

  //Division {} の出力
  if( pfcDivision.Write(fp, 0) != PFC::E_PFC_SUCCESS )
  {
    fclose(fp);
    PFC_PRINT("error write division.(%s)\n", filename.c_str());
    return PFC::E_PFC_ERROR_WRITE_DIVISION;
  }

  fclose(fp);


  return PFC::E_PFC_SUCCESS;
}


// #################################################################
// Division&Region情報の収集＆作成
PFC::E_PFC_ERRORCODE
CPfcCompress::GatherAndCreateDivisionInfo( 
                            CPfcDivision& division  // proc.pfc Division&Regionクラス
                        )
{
  if ( m_myRankID != m_regionMasterRankID ) {
    return PFC::E_PFC_SUCCESS;  // 領域のマスターランクでないランクは関係なし
  }
  
  CPfcRegion region;

  // 複数領域
  if( m_numRegion > 1 ) 
  {
    int msgTag = 2300;
    MPI_Status status;
    int *headtail = NULL;


    int sbuff[6];
    for(int i=0; i<3; i++) {
      sbuff[i]   = m_regionHead[i];
      sbuff[i+3] = m_regionTail[i];
    }

    // 他のマスターランクよりランク０に送信
    if( m_myRankID != 0 )
    {
      MPI_Send( sbuff, 6, 
                MPI_INT, 0, 
                msgTag, m_comm);
    }
    // ランク０の場合、他のマスタランクより受信
    else
    {
      headtail = new int[6*m_numRegion];

      // 自ランク設定
      for(int i=0; i<3; i++) {
        headtail[i]   = sbuff[i];
        headtail[i+3] = sbuff[i+3];
      }
    
      for(int i = 1; i < m_numRegion; i++) {
        MPI_Recv( &headtail[i*6], 6, 
                  MPI_INT, (m_numParallel*i), 
                  msgTag, m_comm, &status );
      }
    }

    if( m_myRankID == 0 ) {
      for(int i=0; i<m_numRegion; i++) {
        region.m_regionID = i;
        for(int j=0; j<3; j++) {
          region.m_headIndex[j]=headtail[i*6+j];
          region.m_tailIndex[j]=headtail[i*6+j+3];
          region.m_voxelSize[j]=region.m_tailIndex[j]-region.m_headIndex[j]+1;

        }
        division.m_regionList.push_back(region);
      }
      
      delete [] headtail;
    }

  }
  // １領域
  else
  {
  
    region.m_regionID = 0;
    for(int i=0; i<3; i++) {
      region.m_headIndex[i]=m_regionHead[i]; 
      region.m_tailIndex[i]=m_regionTail[i];
      region.m_voxelSize[i]=region.m_tailIndex[i]-region.m_headIndex[i]+1;
    }
    division.m_regionList.push_back(region);
  }
  
  return PFC::E_PFC_SUCCESS;
}

