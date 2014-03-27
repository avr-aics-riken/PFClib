/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCioDfiUtil.cpp 
 * @brief  CPfcCioDfiUtil Class
 * @author aics
 */

#include "PfcCioDfiUtil.h"


// #################################################################
// Dfi Domain取得
PFC::E_PFC_ERRORCODE
CPfcCioDfiUtil::GetDfiDomain( const std::string& dfiFilePath, cio_Domain& domain )
{
  PFC::E_PFC_ERRORCODE iret;
  cio_TimeSlice timeSlice;
  
  iret = CPfcCioDfiUtil::GetDfiInfo( dfiFilePath, domain, timeSlice );
  
  return iret;
}


// #################################################################
// DFI タイムステップ　リスト 情報取得
PFC::E_PFC_ERRORCODE
CPfcCioDfiUtil::GetDfiTimeStepList( 
                   const std::string& dfiFilePath,
                   vector<int>&       timeStepList
                 )
{
  PFC::E_PFC_ERRORCODE iret;
  cio_Domain    domain;
  cio_TimeSlice timeSlice;
  

  iret = CPfcCioDfiUtil::GetDfiInfo( dfiFilePath, domain, timeSlice );
  if( iret == PFC::E_PFC_SUCCESS ) {
    for(int i=0; i<timeSlice.SliceList.size();i++) {
      timeStepList.push_back( timeSlice.SliceList[i].step ); 
    }
  }
  
  
  return iret;
}

// #################################################################
// Dfi 情報取得
PFC::E_PFC_ERRORCODE
CPfcCioDfiUtil::GetDfiInfo(
                    const std::string& dfiFilePath,
                    cio_Domain& domain,
                    cio_TimeSlice& timeSlice
                  )
{
  cio_TextParser tpCntl;
  


  /** DFIのディレクトリパスの取得 */
  std::string dirName = CIO::cioPath_DirName(dfiFilePath);


  /** index.dfi read */
  /** TPインスタンス */
  
  tpCntl.getTPinstance();
  
  /** 入力ファイル index.dfi をセット */
  int ierror = 0;
  ierror = tpCntl.readTPfile(dfiFilePath);
  if ( ierror )
  {
    PFC_PRINT("\tinput file not found '%s'\n",dfiFilePath.c_str());
    return PFC::E_PFC_ERROR;
  }

  /** FilePathの読込み: proc.difパス設定 */
  cio_FilePath F_path;
  if( F_path.Read(tpCntl) != CIO::E_CIO_SUCCESS )
  {
    PFC_PRINT("\tFilePath Data Read error %s\n",dfiFilePath.c_str());
    return PFC::E_PFC_ERROR;
  }


  /** TimeSliceの読込み */
  if( timeSlice.Read(tpCntl) != CIO::E_CIO_SUCCESS )
  {
    PFC_PRINT("\tTimeSlice Data Read error %s\n",dfiFilePath.c_str());
    return PFC::E_PFC_ERROR;
  }

  /** proc.dfi file name の取得 */
  std::string dfiname  = CIO::cioPath_FileName(F_path.ProcDFIFile,".dfi");
  std::string procfile = CIO::cioPath_ConnectPath(dirName,dfiname);

  /** proc.dfi read */
  /** TPインスタンス */
  tpCntl.getTPinstance();

  FILE* fp = NULL;
  if( !(fp=fopen(procfile.c_str(),"rb")) ) {
    PFC_PRINT("Can't open file. (%s)\n",procfile.c_str());
    return PFC::E_PFC_ERROR;
  }
  fclose(fp);

  /** 入力ファイル proc.dfi をセット */
  ierror = tpCntl.readTPfile(procfile);
  if ( ierror )
  {
    PFC_PRINT("\tinput file not found '%s'\n",procfile.c_str());
    return PFC::E_PFC_ERROR;
  }

  /** Domainの読込み */

  if( domain.Read(tpCntl) != CIO::E_CIO_SUCCESS ) 
  {
    PFC_PRINT("\tDomain Data Read error %s\n",procfile.c_str());
    return PFC::E_PFC_ERROR;
  }

  /** TextParserの破棄 */
  tpCntl.remove();
  

  return PFC::E_PFC_SUCCESS;

}

