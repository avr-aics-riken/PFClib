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
#include "PfcFileInfo.h"
#include "PfcCompressInfo.h"
#include "PfcFilePath.h"
#include "PfcUnitList.h"
#include "PfcTimeSlice.h"
#include "PfcDivision.h"
#include "PfcDomain.h"

#include "PfcVersion.h"


//#################################################################
// PFCファイル出力( index.pfc )
PFC::E_PFC_ERRORCODE
CPfcCompress::WriteIndexPfcFile( void )
{
  // ランク０のみファイル出力
  if( m_myRankID != 0 ) return PFC::E_PFC_SUCCESS;


  std::string filename = m_prefix;
  filename += ".pfc";
  
  if ( filename.empty() ) return PFC::E_PFC_ERROR_WRITE_INDEXFILENAME_EMPTY;

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
    return PFC::E_PFC_ERROR_WRITE_INDEXFILE_OPENERROR;
  }

  //FileInfo {} の出力
  CPfcFileInfo fileInfo( m_outDirPath,
                         m_prefix,
                         m_fileFormat,
                         0,
                         PFC::E_PFC_FLOAT64,
                         m_endianType,
                         m_arrayShape,
                         m_numComponent );
  
  if( fileInfo.Write(fp, 0) != PFC::E_PFC_SUCCESS )
  {
    fclose(fp);
    return PFC::E_PFC_ERROR_WRITE_FILEINFO;
  }

  int calculatedLayer;
  if( m_compressFormat == "pod" )
  {
    calculatedLayer = m_pCioPod->m_pPod->GetCalculatedLayer();
  }
  else
  {
    calculatedLayer = 0;
  }

  //CompressInfo {} の出力
  CPfcCompressInfo compressInfo (
                    m_compressFormat,
                    m_compressError,
                    calculatedLayer, 
                    POD_COMPRESS_VERSION,
                    m_startStep, 
                    m_endStep
        );

  if( compressInfo.Write(fp, 0) != PFC::E_PFC_SUCCESS )
  {
    fclose(fp);
    return PFC::E_PFC_ERROR_WRITE_COMPRESSINFO;
  }
  
  //FilePath {} の出力
  std::string procFilePath( "proc.pfc" );
  
  CPfcFilePath filePath( m_dfiFilePath, procFilePath );
                         
  if( filePath.Write(fp, 1) != PFC::E_PFC_SUCCESS )
  {
    fclose(fp);
    return PFC::E_PFC_ERROR_WRITE_FILEPATH;
  }

  //UnitList {} の出力
  //  UnitListは cio_Unitと同フォーマットなのでそれを使う
  //const cio_Unit* pCioUnit = m_pDfiIN->GetcioUnit(); 
  cio_Unit* pCioUnit = (cio_Unit*)(m_pDfiIN->GetcioUnit()); 
  if( pCioUnit->Write(fp, 0) != CIO::E_CIO_SUCCESS ) 
  {
    fclose(fp);
    return PFC::E_PFC_ERROR_WRITE_UNITLIST;
  }

  //TimeSlice {} の出力
  const cio_TimeSlice* pCioTimeSlice = m_pDfiIN->GetcioTimeSlice(); 
  CPfcTimeSlice timeSlice( pCioTimeSlice );     // pCioTimeSliceをcopyして作成
  timeSlice.Select( m_startStep, m_endStep );   // TimeSliceを絞る
  
  if ( timeSlice.Write(fp, 1) != CIO::E_CIO_SUCCESS ) 
  {
    fclose(fp);
    return PFC::E_PFC_ERROR_WRITE_TIMESLICE;
  }

  return PFC::E_PFC_SUCCESS;
}

