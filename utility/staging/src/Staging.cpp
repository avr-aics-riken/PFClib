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
 * @file   Staging.C
 * @brief  Staging class 関数
 * @author aics
 */

#include <stdlib.h>
#include "Staging_Utility.h"
#include "PfcFunction.h"

///////////////////////////////////////////////////////////////////
// 初期化、ファイルの読込み
bool Staging::Initial( string infofile, string dfiname )
{


  //infoファイル読込み
  if( !ReadInfo(infofile) ) return false;

  //dfiファイル読込み
  CIO::E_CIO_ERRORCODE ret;
  DFI = cio_DFI::ReadInit(MPI_COMM_WORLD, dfiname, m_GVoxel, m_Gdiv, ret);

  //dfi情報が格納されたクラスポインタの取得
  dfi_Finfo = DFI->GetcioFileInfo();
  dfi_Fpath = DFI->GetcioFilePath();
  dfi_Unit  = DFI->GetcioUnit();
  dfi_Domain= DFI->GetcioDomain();
  dfi_MPI   = DFI->GetcioMPI();
  dfi_TSlice= DFI->GetcioTimeSlice();
  dfi_Process=(cio_Process *)DFI->GetcioProcess();

  if( !m_ActiveSubdomain.empty() ) {
    int divSudomain[3] = {0,0,0};
    //ActiveSubdomainファイルの読込み
    stg_ErrorCode ret = ReadActiveSubdomainFile( m_ActiveSubdomain, m_subDomainInfo, divSudomain);
    if( ret != STG_SUCCESS ) return false;
  } else {
    int nRank = m_GRankInfo.size();
    if( nRank == 0 ) nRank = m_Gdiv[0]*m_Gdiv[1]*m_Gdiv[2];
    //ActiveSubdomainファイルがない場合、全ランク活性ファイルとする
    if( CheckData(nRank) != STG_SUCCESS ) return false; 
  }

  // 全体ステップ数取得
  vector<int> timeStepList;
  for(int i=0; i<dfi_TSlice->SliceList.size();i++) {
    timeStepList.push_back( dfi_TSlice->SliceList[i].step );
  }

  // 対象ステップ設定
  if( m_startStep == -1 ) {
    m_startStep = timeStepList[0];
  }
  if( m_endStep == -1 ) {
    m_endStep = timeStepList[ timeStepList.size()-1 ];
  }
     // 指定ステップ有無チェック
  int ist_pos  = -1;
  int iend_pos = -1;
  for( int i = 0; i<timeStepList.size(); i++ ) {
     if( m_startStep == timeStepList[i] ) {
       ist_pos = i;
     }
     if( m_endStep == timeStepList[i] ) {
       iend_pos = i;
     }
  }

    // 対象のみコピー
  m_stepList.clear();
  for(int i=ist_pos; i<=iend_pos; i++ ) {
    m_stepList.push_back( timeStepList[i] );
  }
  m_numStep = m_stepList.size(); // 対象ステップ数
  if( m_numStep < 2 ) {
    printf("### ERROR  number of steps is short.  m_numStep=%d\n",m_numStep );
    return CIO::E_CIO_ERROR;
  }

  printf("m_numStep           : %d\n",m_numStep);
  printf("m_startStep(update) : %d\n",m_startStep);
  printf("m_endStep  (update) : %d\n",m_endStep);

  // 時間軸方向の並列数（分割領域内のランク数）
  m_numParallel = CPfcFunction::GetPodParallel( m_numStep );
  printf("m_numParallel(time dir.) : %d\n",m_numParallel);

  // PFCの全体ランク数
  m_numRankPfc = m_NumberOfRank*m_numParallel;

  // 領域内のIDごとの担当するステップ数＆ステップ開始位置取得
  for(int i=0; i<m_numParallel; i++) {
    int numStepInId;
    int startStepPos;
    int maxStep;

    // POD 担当するステップ数＆ステップ開始位置取得
    CPfcFunction::GetPodStepInfo(
                  m_numStep,     // (in)  タイムステップ数
                  i,             // (in)  領域内のID(0からの連番）
                  numStepInId,   // (out) idが担当するステップ数
                  startStepPos,  // (out) idの開始ステップ位置(0からの連番）
                  maxStep        // (out) 領域内の各ランクが担当する最大ステップ数
                );
    m_startStepInID.push_back(startStepPos);
    m_numStepInID.push_back(numStepInId);
  }
  // POD ステップ毎の領域内id
  for(int j=0; j<m_numStep; j++) {
    int id=-1;
    for(int i=0; i<m_numParallel; i++) {
      if( j>=m_startStepInID[i] && 
          j< (m_startStepInID[i]+m_numStepInID[i]) )
      {
        id = i;
        break;
      }
    }
    if( id>=0 ) {
      m_idAtStep.push_back(id);
    } else {
      printf("### ERROR  id at step is not found  stepNo=%d\n",j );
      return CIO::E_CIO_ERROR;
    }
  }

  return true;

}

///////////////////////////////////////////////////////////////////
// infoファイルの読込み
bool Staging::ReadInfo(string infofile)
{

  //実行プログラム情報ローダのインスタンス生成
  cio_TextParser tp_stg;
  tp_stg.getTPinstance();

  FILE* fp = NULL;
  if( !(fp=fopen(infofile.c_str(),"rb")) ) {
    printf("Can't open file. (%s)\n",infofile.c_str());
    return false;
  }
  fclose(fp);

  int ierr = tp_stg.readTPfile(infofile);
  if( ierr ) {
    printf("\tinput file not found '%s'\n",infofile.c_str());
    return false;
  }

  string label,label_base,label_leaf;
  int v[3];
  string str;
  int ct;

  //GlobalVoxel
  for (int n=0; n<3; n++) v[n]=0;
  label = "/Domain/GlobalVoxel";
  if( !(tp_stg.GetVector(label, v, 3 )) ) {
    printf("\tParsing error : fail to get '%s'\n",label.c_str());
    return false;
  }
  for(int i=0;i<3;i++ ) m_GVoxel[i]=v[i];

  //GlobalDivision
  for (int n=0; n<3; n++) v[n]=0;
  label = "/Domain/GlobalDivision";
  if( !(tp_stg.GetVector(label, v, 3 )) ) {
    printf("\tParsing error : fail to get '%s'\n",label.c_str());
    return false;
  }
  for(int i=0;i<3;i++ ) m_Gdiv[i]=v[i];

  //ActiveSubdomain
  label = "/Domain/ActiveSubdomainFile";
  if( !(tp_stg.GetValue(label, &str )) ) {
    m_ActiveSubdomain="";
  } else {
    m_ActiveSubdomain=str;
  }

  //NumberOfRank
  label = "/MPI/NumberOfRank";
  if( !(tp_stg.GetValue(label, &ct )) ) {
    m_NumberOfRank=0;
  } else {
    m_NumberOfRank=ct;
  }

  m_GRankInfo.clear();

  Rank rank;
  //Rank
  int nnode=0;
  label_base = "/Process";
  if( tp_stg.chkNode(label_base) ) nnode = tp_stg.countLabels(label_base);

  for (int i=0; i<nnode; i++) {
    if(!tp_stg.GetNodeStr(label_base,i+1,&str))
    {
      printf("\tParsing error : No Elem name\n");
      return false;
    }
    if( strcasecmp(str.substr(0,4).c_str(), "Rank") ) continue;
    label_leaf=label_base+"/"+str;

    //ID
    label = label_leaf + "/ID";
    if ( !(tp_stg.GetValue(label, &ct )) ) {
      printf("\tParsing error : fail to get '%s'\n",label.c_str());
      return false;
    }
    else {
      rank.RankID= ct;
    }

    //VoxelSize
    label = label_leaf + "/VoxelSize";
    for (int n=0; n<3; n++) v[n]=0;
    if ( !(tp_stg.GetVector(label, v, 3 )) ) {
      printf("\tParsing error : fail to get '%s'\n",label.c_str());
      return false;
    }
    for (int n=0; n<3; n++) rank.VoxelSize[n]=v[n];

    //HeadIndex
    label = label_leaf + "/HeadIndex";
    for (int n=0; n<3; n++) v[n]=0;
    tp_stg.GetVector(label, v, 3 ); 
    for (int n=0; n<3; n++) rank.HeadIndex[n]=v[n];

    //TailIndex
    label = label_leaf + "/TailIndex";
    for (int n=0; n<3; n++) v[n]=0;
    tp_stg.GetVector(label, v, 3 ); 
    for (int n=0; n<3; n++) rank.TailIndex[n]=v[n];

    //RankPosition
    label = label_leaf + "/RankPosition";
    for (int n=0; n<3; n++) v[n]=-1;
    tp_stg.GetVector(label, v, 3 ); 
    for (int n=0; n<3; n++) rank.RankPosition[n]=v[n]; 

    m_GRankInfo.push_back(rank);

  }

  tp_stg.remove();

  return true;
}

///////////////////////////////////////////////////////////////////
// 領域分割数の取得
const int* Staging::GetDivNum() const
{
  return m_Gdiv;
}

///////////////////////////////////////////////////////////////////
// 活性サブドメイン情報の存在チェック
bool Staging::IsExistSubdomain( ActiveSubDomain subDomain)
{
  for( size_t i=0;i<m_subDomainInfo.size();i++ )
  {
    ActiveSubDomain dom = m_subDomainInfo[i];
    if( dom == subDomain ) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// 活性サブドメイン情報の追加
bool Staging::AddSubdomain(ActiveSubDomain subDomain )
{

  //既存チェック
  if( IsExistSubdomain(subDomain) ) return false;

  //追加
  m_subDomainInfo.push_back(subDomain);
  return true;
}

///////////////////////////////////////////////////////////////////
// 活性サブドメインの数を取得
int Staging::GetSubdomainNum() const
{
  if( m_subDomainInfo.size() > 0 )
  {
    return (int)m_subDomainInfo.size();
  }
  if( m_Gdiv[0] <= 0 || m_Gdiv[1] <= 0 || m_Gdiv[2] <= 0 )
  {
    return 0;
  }
  return m_Gdiv[0] * m_Gdiv[1] * m_Gdiv[2];
}

///////////////////////////////////////////////////////////////////
// 活性サブドメインの数を取得
const ActiveSubDomain* Staging::GetSubdomainInfo( size_t idx ) const
{
  if( int(idx) >= GetSubdomainNum() ) return NULL;
  return &(m_subDomainInfo[idx]);
}

///////////////////////////////////////////////////////////////////
// ランクマップを生成
//bool Staging::CreateRankMap()
int* Staging::CreateRankMap()
{

  //m_rankMap = NULL;

  // 領域分割数を取得
  const int* div = GetDivNum();
  if( !div ) return false;

  // マップ領域を確保(初期値NULL)
  size_t ndiv = size_t(div[0]) * size_t(div[1]) * size_t(div[2]);
  int *rankMap = new int[ndiv];
  if( !rankMap )
  {
    return false;
  }
  for( size_t i=0;i<ndiv;i++ ) rankMap[i] = -1;

  // 活性サブドメイン情報配置位置に0をセット
  for( int i=0;i<GetSubdomainNum();i++ )
  {
    //サブドメイン情報
    const ActiveSubDomain* dom = GetSubdomainInfo(i);
    if( !dom )
    {
      delete [] rankMap;
      return false;
    }

    // 位置を取得
    const int *pos = dom->GetPos();
    if( !pos )
    {
      delete [] rankMap;
      return false;
    }

    // 0をセット
    rankMap[_IDX_S3D(pos[0],pos[1],pos[2],div[0],div[1],div[2],0)] = 0;
  }

  bool flg = true;
  for(int i=0; i<m_GRankInfo.size(); i++)
  {
    if( m_GRankInfo[i].RankPosition[0]<0 || m_GRankInfo[i].RankPosition[1]<0 || 
        m_GRankInfo[i].RankPosition[2]<0 ) {
      flg = false;
      break;
    }
  }

  if( !flg || m_GRankInfo.size()<1 ) {
    // i->j->kの優先順で活性サブドメインにランク番号をセット
    int rankCount = 0;
    for( int k=0;k<div[2];k++ ){
    for( int j=0;j<div[1];j++ ){
    for( int i=0;i<div[0];i++ ){
      if( rankMap[_IDX_S3D(i,j,k,div[0],div[1],div[2],0)] == 0 )
      {
        rankMap[_IDX_S3D(i,j,k,div[0],div[1],div[2],0)] = rankCount;
        rankCount++;
      }
    }}}
  } else {
    //printf("RankPosition set\n");
    // i->j->kの優先順で活性サブドメインにランク番号をセット
    for( int n=0; n<m_GRankInfo.size(); n++ ) {
      int i=m_GRankInfo[n].RankPosition[0];
      int j=m_GRankInfo[n].RankPosition[1];
      int k=m_GRankInfo[n].RankPosition[2];
      if( rankMap[_IDX_S3D(i,j,k,div[0],div[1],div[2],0)] == 0 )
      {
        rankMap[_IDX_S3D(i,j,k,div[0],div[1],div[2],0)] = n;
      }
    }
  }

  // ランクマップをセット
  //if( m_rankMap ) delete [] m_rankMap;
  //m_rankMap = rankMap;

  //for(int i=0;i<ndiv;i++) printf("rankMap[%d] : %d\n",i,rankMap[i]);

  return rankMap;
}

///////////////////////////////////////////////////////////////////
// ランクマップを生成
int* Staging::CreateActiveRankMap()
{

  int i,j,k;

  int div[3];
  div[0]=m_mapX.size();
  div[1]=m_mapY.size();
  div[2]=m_mapZ.size();

  size_t ndiv = div[0]*div[1]*div[2];

  int *rankMap = new int[ndiv];
  for(int i=0; i<ndiv; i++) rankMap[i]=-1;

  headT::iterator it;

  for(int n=0; n<m_GRankInfo.size(); n++)
  {
    it=m_mapX.find(m_GRankInfo[n].HeadIndex[0]);
    i=it->second;

    it=m_mapY.find(m_GRankInfo[n].HeadIndex[1]);
    j=it->second;

    it=m_mapZ.find(m_GRankInfo[n].HeadIndex[2]);
    k=it->second;

    rankMap[_IDX_S3D(i,j,k,div[0],div[1],div[2],0)] = n;
  }

  return rankMap; 

}

///////////////////////////////////////////////////////////////////
// ActiveSubdomainファイルのエンディアンチェック 
stg_EMatchType Staging::isMatchEndianSbdmMagick( int ident )
{
  char magick_c[] = "SBDM";
  int  magick_i=0;

  //cheak match
  magick_i = (magick_c[3]<<24) + (magick_c[2]<<16) + (magick_c[1]<<8) + magick_c[0];
  if( magick_i == ident )
  {
    return STG_Match;
  }

  //chack unmatch
  magick_i = (magick_c[0]<<24) + (magick_c[1]<<16) + (magick_c[2]<<8) + magick_c[3];
  if( magick_i == ident )
  {
    return STG_UnMatch;
  }

  //unknown format
  return STG_UnKnown;
}


///////////////////////////////////////////////////////////////////
// ActiveSubdomainファイルの読み込み
stg_ErrorCode Staging::ReadActiveSubdomainFile( std::string subDomainFile )
{

  //読込み
  int div[3];
  stg_ErrorCode ret = ReadActiveSubdomainFile( subDomainFile, m_subDomainInfo, div );
  if( ret != STG_SUCCESS ) return ret;

  // 領域分割数のチェック
  if( m_Gdiv[0] > 0 && m_Gdiv[1] > 0 && m_Gdiv[2] > 0 )
  {
    if( div[0] != m_Gdiv[0] || div[0] != m_Gdiv[0] ||div[0] != m_Gdiv[0] )
    {
      return STG_ERROR_MISMATCH_DIV_SUBDOMAIN;
    }
  } 

  return STG_SUCCESS;
}


///////////////////////////////////////////////////////////////////
// ActiveSubdomainファイルの読み込み(static関数)
stg_ErrorCode Staging::ReadActiveSubdomainFile( std::string subDomainFile,
                                                std::vector<ActiveSubDomain>& subDomainInfo,
                                                int div[3] )
{
  if( subDomainFile.empty() ) return STG_ERROR_OPEN_SBDM;

  // ファイルオープン
  FILE*fp = fopen( subDomainFile.c_str(), "rb" );
  if( !fp ) return STG_ERROR_OPEN_SBDM; 

  //エンディアン識別子
  int ident;
    if( fread( &ident, sizeof(int), 1, fp ) != 1 )
  {
    fclose(fp);
    return STG_ERROR_READ_SBDM_HEADER;
  }

  stg_EMatchType endian = isMatchEndianSbdmMagick( ident );
  if( endian == STG_UnKnown )
  {
    fclose(fp);
    return STG_ERROR_READ_SBDM_FORMAT;
  }

  // 領域分割数
  if( fread( div, sizeof(int), 3, fp ) != 3 )
  {
    fclose(fp);
    return STG_ERROR_READ_SBDM_DIV;
  }
  if( endian == STG_UnMatch ) BSWAPVEC(div,3);

  // contents
  size_t nc = size_t(div[0]) * size_t(div[1]) * size_t(div[2]);
  unsigned char *contents = new unsigned char[nc];
  if( fread( contents, sizeof(unsigned char), nc, fp ) != nc )
  {
    delete [] contents;
    fclose(fp);
    return STG_ERROR_READ_SBDM_CONTENTS;
  }

  // ファイルクローズ
  fclose(fp);

  size_t ptr = 0;
  // 活性ドメイン情報の生成
  for( int k=0;k<div[2];k++ ){
  for( int j=0;j<div[1];j++ ){
  for( int i=0;i<div[0];i++ ){
    if( contents[ptr] == 0x01 )
    {
      int pos[3] = {i,j,k};
      ActiveSubDomain dom( pos );
      subDomainInfo.push_back(dom);
    }
    ptr++;
  }}}

  // contentsのdelete
  delete [] contents;

  // 活性ドメインの数をチェック
  if( subDomainInfo.size() == 0 )
  {
    return STG_ERROR_SBDM_NUMDOMAIN_ZERO;
  }
 
  return STG_SUCCESS;
}
 
///////////////////////////////////////////////////////////////////
// 領域情報のチェック
stg_ErrorCode Staging::CheckData( int nRank )
{

  // 領域分割数
  if( m_Gdiv[0] <= 0 || m_Gdiv[1] <= 0 || m_Gdiv[2] <= 0 ) 
    return STG_ERROR_INVALID_DIVNUM;

  //活性サブドメイン情報
  int ndom = m_subDomainInfo.size();

  bool flg=true;
  for( int i=0; i<m_GRankInfo.size(); i++ ) {
    if( m_GRankInfo[i].RankPosition[0]<0 || m_GRankInfo[i].RankPosition[1]<0 ||
        m_GRankInfo[i].RankPosition[2]<0 ) {
      flg=false;
      break;
    }
  }

  if( ndom==0 && m_GRankInfo.size()==0 ) flg=false;

  if( ndom == 0 ) {
    //活性サブドメイン情報が空のとき、全領域を活性サブドメインとする
    //if( nRank != m_Gdiv[0]*m_Gdiv[1]*m_Gdiv[2] )
    //{
    //  return STG_ERROR_MISMATCH_NP_SUBDOMAIN;
    //}
    if( flg ) {
      for( int n=0; n<m_GRankInfo.size(); n++ ) {
        int i=m_GRankInfo[n].RankPosition[0];
        int j=m_GRankInfo[n].RankPosition[1];
        int k=m_GRankInfo[n].RankPosition[2];
        int pos[3] = {i,j,k};
        ActiveSubDomain dom( pos );
        AddSubdomain( dom );
      }
    } else {
      for( int k=0;k<m_Gdiv[2];k++ ){
      for( int j=0;j<m_Gdiv[1];j++ ){
      for( int i=0;i<m_Gdiv[0];i++ ){
        int pos[3] = {i,j,k};
        ActiveSubDomain dom( pos );
        AddSubdomain( dom );
      }}}
    }
  } else {
    //if( nRank != ndom )
    //{
    //  return STG_ERROR_MISMATCH_NP_SUBDOMAIN;
    //}
  }

  return STG_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// VOXEL数の取得
const int* Staging::GetVoxNum() const
{
  return m_GVoxel;
}

///////////////////////////////////////////////////////////////////
// head&tail の生成
bool Staging::CreateHeadTail(int* rankMap, vector<Rank>& RankInfo)
{

  if( !rankMap ) return false;
 
  //領域分割数
  const int *div = GetDivNum();
  if( !div ) return false;
 
  int numrank=0;
  for(int i=0; i<div[0]*div[1]*div[2]; i++ ) {
    if( rankMap[i] >= 0 ) numrank++;
  }

  if( numrank != RankInfo.size() ) {
    printf("ERROR MISMATCH NumberOfRank\n");
    return false;
  }
 
  bool flg=true;
  for( int i=0; i<RankInfo.size(); i++ ) {
    if( RankInfo[i].HeadIndex[0]<1 || RankInfo[i].HeadIndex[1]<1 || RankInfo[i].HeadIndex[2]<1 )
    {
      flg=false;
      break;
    }
  }

  if( flg ) return true;


  //全体ボクセル数
  const int *gvox = GetVoxNum();
  if( !gvox ) return false;

  // ローカルのVOXEL数
  int *nvX = new int[div[0]];
  int *nvY = new int[div[1]];
  int *nvZ = new int[div[2]];
  int *nv[3] = {nvX,nvY,nvZ};
  for( int n=0;n<3;n++ )
  {
    int *nvd = nv[n];
    //基準のボクセル数
    int nbase = gvox[n] / div[n];

    //余り
    int amari = gvox[n] % div[n];

    //ボクセル数をセット
    for( int i=0;i<div[n];i++ )
    {
      nvd[i] = nbase;
      if( i<amari ) nvd[i]++;
    }
  }

  //head
  int *headX = new int[div[0]];
  int *headY = new int[div[1]];
  int *headZ = new int[div[2]];
  int *head[3] = {headX,headY,headZ};
  for( int n=0;n<3;n++ )
  {
    int *nvd = nv[n];
    int *hd = head[n];
    hd[0] = 1;

    for( int i=1;i<div[n];i++ )
    {
      hd[i] = hd[i-1]+nvd[i-1];
    }
  }

  for( int k=0;k<div[2];k++ ){
  for( int j=0;j<div[1];j++ ){
  for( int i=0;i<div[0];i++ ){
    int rankNo = rankMap[_IDX_S3D(i,j,k,div[0],div[1],div[2],0)];

    if( rankNo < 0 ) continue;

    m_HeadTail[rankNo][0] = headX[i];
    m_HeadTail[rankNo][1] = headY[j];
    m_HeadTail[rankNo][2] = headZ[k];
    m_HeadTail[rankNo][3] = headX[i]+nvX[i]-1;
    m_HeadTail[rankNo][4] = headY[j]+nvY[j]-1;
    m_HeadTail[rankNo][5] = headZ[k]+nvZ[k]-1;

    RankInfo[rankNo].RankID=rankNo;
    RankInfo[rankNo].VoxelSize[0]=m_HeadTail[rankNo][3]-m_HeadTail[rankNo][0]+1;
    RankInfo[rankNo].VoxelSize[1]=m_HeadTail[rankNo][4]-m_HeadTail[rankNo][1]+1;
    RankInfo[rankNo].VoxelSize[2]=m_HeadTail[rankNo][5]-m_HeadTail[rankNo][2]+1;
    RankInfo[rankNo].HeadIndex[0]=m_HeadTail[rankNo][0];
    RankInfo[rankNo].HeadIndex[1]=m_HeadTail[rankNo][1];
    RankInfo[rankNo].HeadIndex[2]=m_HeadTail[rankNo][2];
    RankInfo[rankNo].TailIndex[0]=m_HeadTail[rankNo][3];
    RankInfo[rankNo].TailIndex[1]=m_HeadTail[rankNo][4];
    RankInfo[rankNo].TailIndex[2]=m_HeadTail[rankNo][5];

  }}}

  delete [] nvX;
  delete [] nvY;
  delete [] nvZ;
  delete [] headX;
  delete [] headY;
  delete [] headZ;

  return true;
}
///////////////////////////////////////////////////////////////////
// head&tail の生成
bool Staging::CreateHeadTail(int* rankMap)
{

  if( !rankMap ) return false;

  //領域分割数
  const int *div = GetDivNum();
  if( !div ) return false;

  //全体ボクセル数
  const int *gvox = GetVoxNum();
  if( !gvox ) return false;

  // ローカルのVOXEL数
  int *nvX = new int[div[0]];
  int *nvY = new int[div[1]];
  int *nvZ = new int[div[2]];
  int *nv[3] = {nvX,nvY,nvZ};
  for( int n=0;n<3;n++ )
  {
    int *nvd = nv[n];
    //基準のボクセル数
    int nbase = gvox[n] / div[n];

    //余り
    int amari = gvox[n] % div[n];

    //ボクセル数をセット
    for( int i=0;i<div[n];i++ )
    {
      nvd[i] = nbase;
      if( i<amari ) nvd[i]++;
    }
  }

  //head
  int *headX = new int[div[0]];
  int *headY = new int[div[1]];
  int *headZ = new int[div[2]];
  int *head[3] = {headX,headY,headZ};
  for( int n=0;n<3;n++ )
  {
    int *nvd = nv[n];
    int *hd = head[n];
    hd[0] = 1;

    for( int i=1;i<div[n];i++ )
    {
      hd[i] = hd[i-1]+nvd[i-1];
    }
  }

  Rank rank;

  for( int k=0;k<div[2];k++ ){
  for( int j=0;j<div[1];j++ ){
  for( int i=0;i<div[0];i++ ){
    int rankNo = rankMap[_IDX_S3D(i,j,k,div[0],div[1],div[2],0)];

    if( rankNo < 0 ) continue;

    m_HeadTail[rankNo][0] = headX[i];
    m_HeadTail[rankNo][1] = headY[j];
    m_HeadTail[rankNo][2] = headZ[k];
    m_HeadTail[rankNo][3] = headX[i]+nvX[i]-1;
    m_HeadTail[rankNo][4] = headY[j]+nvY[j]-1;
    m_HeadTail[rankNo][5] = headZ[k]+nvZ[k]-1;

    rank.RankID=rankNo;
    rank.VoxelSize[0]=m_HeadTail[rankNo][3]-m_HeadTail[rankNo][0]+1;
    rank.VoxelSize[1]=m_HeadTail[rankNo][4]-m_HeadTail[rankNo][1]+1;
    rank.VoxelSize[2]=m_HeadTail[rankNo][5]-m_HeadTail[rankNo][2]+1;
    rank.HeadIndex[0]=m_HeadTail[rankNo][0];
    rank.HeadIndex[1]=m_HeadTail[rankNo][1];
    rank.HeadIndex[2]=m_HeadTail[rankNo][2];
    rank.TailIndex[0]=m_HeadTail[rankNo][3];
    rank.TailIndex[1]=m_HeadTail[rankNo][4];
    rank.TailIndex[2]=m_HeadTail[rankNo][5];
    m_GRankInfo.push_back(rank);

  }}}

  delete [] nvX;
  delete [] nvY;
  delete [] nvZ;
  delete [] headX;
  delete [] headY;
  delete [] headZ;

  return true;
}

///////////////////////////////////////////////////////////////////
// head&tail の生成
void Staging::SetHeadTail()
{
  for(int i=0; i<m_GRankInfo.size(); i++) {
    m_HeadTail[i][0]=m_GRankInfo[i].HeadIndex[0];
    m_HeadTail[i][1]=m_GRankInfo[i].HeadIndex[1];
    m_HeadTail[i][2]=m_GRankInfo[i].HeadIndex[2];
    m_HeadTail[i][3]=m_GRankInfo[i].TailIndex[0];
    m_HeadTail[i][4]=m_GRankInfo[i].TailIndex[1];
    m_HeadTail[i][5]=m_GRankInfo[i].TailIndex[2];

    m_headX.insert(m_HeadTail[i][0]);
    m_headY.insert(m_HeadTail[i][1]);
    m_headZ.insert(m_HeadTail[i][2]);
  }
 
}

///////////////////////////////////////////////////////////////////
// head map の生成
void Staging::CreateHeadMap(std::set<int>head, headT &map)
{

  int cnt=0;
  for(std::set<int>::iterator it=head.begin(); it!=head.end(); it++)
  {
    int key=*it;
    map.insert(headT::value_type(key,cnt));
    cnt++;
  }

}
///////////////////////////////////////////////////////////////////
// 粗密判定
stg_EGlobalVoxel Staging::CheckGlobalVoxel(int Gvoxel[3], int DFI_Gvoxel[3])
{

  if( Gvoxel[0] == DFI_Gvoxel[0]   &&
      Gvoxel[1] == DFI_Gvoxel[1]   &&
      Gvoxel[2] == DFI_Gvoxel[2]   ) return STG_E_GV_SAME;

  if( Gvoxel[0] == DFI_Gvoxel[0]*2 &&
      Gvoxel[1] == DFI_Gvoxel[1]*2 &&
      Gvoxel[2] == DFI_Gvoxel[2]*2 ) return STG_E_GVX2_SAME;

  return STG_E_OTHER;
}

/*
///////////////////////////////////////////////////////////////////
// 密HeadTail作成（DFI)
void Staging::MakeGVX2HeadTeail(int (*HeadTail)[6], int numrank)
{

  for(int i=0; i<numrank; i++ ) {
    HeadTail[i][0] = DFI->DFI_HeadTail[i][0]*2-1;
    HeadTail[i][1] = DFI->DFI_HeadTail[i][1]*2-1;
    HeadTail[i][2] = DFI->DFI_HeadTail[i][2]*2-1;
    HeadTail[i][3] = DFI->DFI_HeadTail[i][3]*2;
    HeadTail[i][4] = DFI->DFI_HeadTail[i][4]*2;
    HeadTail[i][5] = DFI->DFI_HeadTail[i][5]*2;
  }

}


///////////////////////////////////////////////////////////////////
// 検索範囲のしぼりこみ
bool Staging::CreateStartEnd(int (*HeadTail)[6])
{

  int div[3];
  div[0] = DFI->DFI_mapX.size();
  div[1] = DFI->DFI_mapY.size();
  div[2] = DFI->DFI_mapZ.size();

  std::set<int>headx,heady,headz;
  for(int k=0; k<div[2]; k++) {
  for(int j=0; j<div[1]; j++) {
  for(int i=0; i<div[0]; i++) {
    int rank = DFI->DFI_rankMap[_IDX_S3D(i,j,k,div[0],div[1],div[2],0)];
    headx.insert(HeadTail[rank][0]);
    heady.insert(HeadTail[rank][1]);
    headz.insert(HeadTail[rank][2]);
  }}}

  headT mapx,mapy,mapz;
  CreateHeadMap(headx,mapx);
  CreateHeadMap(heady,mapy);
  CreateHeadMap(headz,mapz);

  int gdiv[3];
  gdiv[0]=m_mapX.size();
  gdiv[1]=m_mapY.size();
  gdiv[2]=m_mapZ.size();
  for( int k=0; k<gdiv[2]; k++ ) {
  for( int j=0; j<gdiv[1]; j++ ) {
  for( int i=0; i<gdiv[0]; i++ ) {
   int myRank = m_rankMap[_IDX_S3D(i,j,k,gdiv[0],gdiv[1],gdiv[2],0)]; 
   if( myRank<0 ) continue;

   //x方向のしぼりこみ
   int sta=m_HeadTail[myRank][0];
   int end=m_HeadTail[myRank][3];

   for( std::set<int>::iterator it=headx.begin(); it!=headx.end(); it++ )
   {
     if( sta >= *it ) m_StartEnd[myRank][0]=mapx.find(*it)->second;
     if( end >= *it ) m_StartEnd[myRank][3]=mapx.find(*it)->second;
   } 

   //y方向のしぼりこみ
   sta=m_HeadTail[myRank][1];
   end=m_HeadTail[myRank][4];

   for( std::set<int>::iterator it=heady.begin(); it!=heady.end(); it++ )
   {
     if( sta >= *it ) m_StartEnd[myRank][1]=mapy.find(*it)->second;
     if( end >= *it ) m_StartEnd[myRank][4]=mapy.find(*it)->second;
   } 

   //z方向のしぼりこみ
   sta=m_HeadTail[myRank][2];
   end=m_HeadTail[myRank][5];

   for( std::set<int>::iterator it=headz.begin(); it!=headz.end(); it++ )
   {
     if( sta >= *it ) m_StartEnd[myRank][2]=mapz.find(*it)->second;
     if( end >= *it ) m_StartEnd[myRank][5]=mapz.find(*it)->second;
   } 


  }}}


  if( headx.size()>0 ) headx.clear();
  if( heady.size()>0 ) heady.clear();
  if( headz.size()>0 ) headz.clear();
  if( mapx.size()>0 ) mapx.clear();
  if( mapy.size()>0 ) mapy.clear();
  if( mapz.size()>0 ) mapz.clear();

  return true; 

}
*/
///////////////////////////////////////////////////////////////////
// ファイルコピー
bool Staging::FileCopy(
         vector<int>readRankList,   // [in] CIOのランクリスト
         int myRegion               // [in] PFCの領域ID
      )
{

  bool mio;
  if( dfi_Process->RankList.size()>1 ) mio=true;
  else mio=false;

  char*cmd = new char[512];
  char tmp[20];

  if( m_outPath == "" ) m_outPath=".";
  int len = m_outPath.size()+7;

  string fname_dfi;

  // ディレクトリ作成
  for(int i=0; i<m_numParallel; i++) {
    int rank_pfc = m_numParallel*myRegion + i;
    sprintf(tmp,"%06d", rank_pfc);
    string path = m_outPath + string("/") + string(tmp);
    MakeDirectory(path);
#if 0
    if( dfi_Finfo->DirectoryPath==""   || 
        dfi_Finfo->DirectoryPath=="./"    ) {
    } else {
      string path_fieldDir = path + string("/") + dfi_Finfo->DirectoryPath;
      MakeDirectory(path_fieldDir);
    }
#endif
  }

  // stepのループ
  for(int j=0; j<m_stepList.size(); j++) {
    int step       = m_stepList[j];
    int idInRegion = m_idAtStep[j];  // 領域内のID
    int rank_pfc   = m_numParallel*myRegion + idInRegion; // PFCのランク

    sprintf(tmp,"%06d", rank_pfc);
#if 0
    string path =  m_outPath + string("/")
                 + string(tmp) + string("/") 
                 + dfi_Finfo->DirectoryPath + string("/");
#else
    string path =  m_outPath + string("/")
                 + string(tmp) + string("/");
#endif

    string path2;
    if( dfi_Finfo->TimeSliceDirFlag == CIO::E_CIO_ON ) {
      sprintf(tmp,"%010d",step);
      path2 = path+tmp;
      MakeDirectory(path2);
    } else {
      path2 = path;
    }

    // copy *.sph or *.bov
    for(int i=0; i<readRankList.size(); i++) {
      fname_dfi = CIO::cioPath_ConnectPath(m_inPath,Generate_FileName(readRankList[i],step,mio)); 
      memset(cmd, 0, sizeof(char)*512 );
      sprintf(cmd,"cp %s %s\n",fname_dfi.c_str(),path2.c_str());
      //printf("  FileCopy() 1 cmd=%s\n",cmd);
      system(cmd);
    }
  }

  // copy proc.dfi, pfc_cntl
  for(int i=0; i<m_numParallel; i++) {
    int rank_pfc = m_numParallel*myRegion + i;
    sprintf(tmp,"%06d", rank_pfc);
    string path = m_outPath + string("/") + string(tmp);

    // proc.dfi
    memset(cmd, 0, sizeof(char)*512 );
    std::string procfname = CIO::cioPath_ConnectPath(m_inPath,dfi_Fpath->ProcDFIFile);
    sprintf(cmd,"cp %s %s/%s_proc.dfi\n",procfname.c_str(),
          path.c_str(),dfi_Finfo->Prefix.c_str());
    //printf("  FileCopy() 2 cmd=%s\n",cmd);
    system(cmd);

    // pfc_cntl
    //   圧縮制御データが指定されていた場合、ランク毎のディレクトリにコピーする
    if( m_cntlPath.empty() ) {
      // no operation
    } else {
      memset(cmd, 0, sizeof(char)*512 );
      sprintf(cmd,"cp %s %s\n", m_cntlPath.c_str(), path.c_str());
      system(cmd);
    }
  }

  if( cmd ) delete [] cmd;

  return true;
}

///////////////////////////////////////////////////////////////////
// dfiファイル出力
// PFC用
bool Staging::OutputDFI(string fname, int* rankMap)
{
  if( m_outPath == "" ) m_outPath="./";
  int len = m_outPath.size()+7;
  char tmp[20];

  string fname_dfi;

  for(int k=0; k<m_Gdiv[2]; k++) {
  for(int j=0; j<m_Gdiv[1]; j++) {
  for(int i=0; i<m_Gdiv[0]; i++) {
    int myRegion = rankMap[_IDX_S3D(i,j,k,m_Gdiv[0],m_Gdiv[1],m_Gdiv[2],0)];
    if( myRegion < 0 ) continue;

    for(int jj=0; jj<m_stepList.size(); jj++) {
      int idInRegion = m_idAtStep[jj];  // 領域内のID
      int rank_pfc   = m_numParallel*myRegion + idInRegion; // PFCのランク

      sprintf(tmp,"%06d",rank_pfc);
      string path =m_outPath + string("/") + string(tmp);

      //MakeDirectory(path);
      path += string("/");

      string DfiName = path+CIO::cioPath_FileName(fname,".dfi");

      WriteIndexDfiFile(DfiName);
    }

  }
  }
  }

  return true;
}

///////////////////////////////////////////////////////////////////
// ファイル名を作成
std::string Staging::Generate_FileName(int RankID, int step, const bool mio)
{

  if( dfi_Finfo->DirectoryPath.empty() ) return NULL;
  if( dfi_Finfo->Prefix.empty() ) return NULL;

  string fmt;
  if( dfi_Finfo->FileFormat == CIO::E_CIO_FMT_SPH ) {
    fmt=D_CIO_EXT_SPH;
  } else if( dfi_Finfo->FileFormat == CIO::E_CIO_FMT_BOV ) {
    fmt=D_CIO_EXT_BOV;
  }

  int len = dfi_Finfo->DirectoryPath.size() + dfi_Finfo->Prefix.size() +
            fmt.size() + 25;

  if( dfi_Finfo->TimeSliceDirFlag == CIO::E_CIO_ON ) len += 11;

  char* tmp = new char[len];
  memset(tmp, 0, sizeof(char)*len);

  if( mio ) {
    if( dfi_Finfo->TimeSliceDirFlag == CIO::E_CIO_ON ) {
      sprintf(tmp, "%s/%010d/%s_%010d_id%06d.%s",dfi_Finfo->DirectoryPath.c_str(),
                                         step,dfi_Finfo->Prefix.c_str(),
                                         step,RankID,fmt.c_str());
    } else {
      sprintf(tmp, "%s/%s_%010d_id%06d.%s",dfi_Finfo->DirectoryPath.c_str(),
                                         dfi_Finfo->Prefix.c_str(),
                                         step,RankID,fmt.c_str());
    }
  } else {
    sprintf(tmp, "%s/%s_%010d.%s",dfi_Finfo->DirectoryPath.c_str(),
                                  dfi_Finfo->Prefix.c_str(),
                                  step,fmt.c_str());
  }
  std::string fname(tmp);
  if( tmp ) delete [] tmp;

  return fname;
}
///////////////////////////////////////////////////////////////////
// ディレクトリがなければ作成、既存なら何もしない
int Staging::MakeDirectory(string path)
{
  int ret = MakeDirectorySub(path);
  if( ret != 0 )
  {
     // 既存以外のエラー
     if ( EEXIST != errno )
     {
        printf( "\tError(errno)=[%s]\n", strerror(errno) );
        return 0;
     }
  }

  return 1;
}

///////////////////////////////////////////////////////////////////
// ディレクトリがなければ作成、既存なら何もしない
int Staging::MakeDirectoryPath()
{

  std::string path = Generate_Directory_Path();

  return MakeDirectory(path);
}

///////////////////////////////////////////////////////////////////
int Staging::MakeDirectorySub( std::string path )
{

  umask(022);

  int ret = mkdir(path.c_str(), 0777);
  if( ret != 0 )
  {
    if( errno == EEXIST ) return 0;

    std::string parent = CIO::cioPath_DirName(path);
    int ret2 = MakeDirectorySub( parent );
    if( ret2 != 0 )
    {
      return ret2;
    }
    ret = MakeDirectorySub( path );
  }

  return ret;

}

// #################################################################
// Directoryパスを生成する関数
std::string Staging::Generate_Directory_Path()
{

  // dfiのパスとDirectoryPathを連結する関数
  // ただし、絶対パスのときはdfiのパスは無視
  // CIO::cioPath_isAbsoluteがtrueのとき絶対パス
  // DirectoryPath + TimeSliceDir
  std::string path = m_inPath;
  if( dfi_Finfo->TimeSliceDirFlag == CIO::E_CIO_ON )
  {
    path = CIO::cioPath_ConnectPath(path, "");
  }

  if( CIO::cioPath_isAbsolute(path) )
  {
    return path;
  }

  path = CIO::cioPath_ConnectPath(m_inPath, path);
  return path;

}

// #################################################################
// Index DFIファイルの出力
CIO::E_CIO_ERRORCODE
Staging::WriteIndexDfiFile(const std::string dfi_name)
{
  if ( dfi_name.empty() ) return CIO::E_CIO_ERROR_WRITE_INDEXFILENAME_EMPTY;
  if ( dfi_Finfo->Prefix.empty() ) return CIO::E_CIO_ERROR_WRITE_PREFIX_EMPTY;

  FILE* fp = NULL;

  // File exist ?
  bool flag = false;
  if ( fp = fopen(dfi_name.c_str(), "r") )
  {
    flag = true;
    fclose(fp);
  }

  if( !(fp = fopen(dfi_name.c_str(), "w")) )
  {
    fprintf(stderr, "Can't open file.(%s)\n", dfi_name.c_str());
    return CIO::E_CIO_ERROR_WRITE_INDEXFILE_OPENERROR;
  }

  //FileInfo {} の出力
#if 0
  cio_FileInfo *t_Finfo = (cio_FileInfo *)dfi_Finfo;
  if( t_Finfo->Write(fp, 0) != CIO::E_CIO_SUCCESS )
  {
    fclose(fp);
    return CIO::E_CIO_ERROR_WRITE_FILEINFO;
  }
#else
  std::string outDirPath("./");

  cio_FileInfo fileInfo ( 
//> add 2014.02.11
                dfi_Finfo->DFIType,
                dfi_Finfo->FieldFilenameFormat,
//< add 2014.02.11
                outDirPath,
                dfi_Finfo->TimeSliceDirFlag,
                dfi_Finfo->Prefix,
                dfi_Finfo->FileFormat,
                dfi_Finfo->GuideCell,
                dfi_Finfo->DataType,
                dfi_Finfo->Endian,
                dfi_Finfo->ArrayShape,
                dfi_Finfo->Component
              );
  for(int i=0; i<dfi_Finfo->ComponentVariable.size();i++ ) {
    fileInfo.ComponentVariable.push_back( dfi_Finfo->ComponentVariable[i] );
  }
  if( fileInfo.Write(fp, 0) != CIO::E_CIO_SUCCESS )
  {
    fclose(fp);
    return CIO::E_CIO_ERROR_WRITE_FILEINFO;
  }
#endif

  //FilePath {} の出力
  cio_FilePath t_Fpath;
  t_Fpath.ProcDFIFile = dfi_Finfo->Prefix+"_proc.dfi";
  if( t_Fpath.Write(fp, 1) != CIO::E_CIO_SUCCESS )
  {
    fclose(fp);
    return CIO::E_CIO_ERROR_WRITE_FILEPATH;
  }

  //Unit {} の出力
  cio_Unit *t_Unit = (cio_Unit *)dfi_Unit;
  if( t_Unit->Write(fp, 0) != CIO::E_CIO_SUCCESS )
  {
    fclose(fp);
    return CIO::E_CIO_ERROR_WRITE_UNIT;
  }

  //TimeSlice {} の出力
  cio_TimeSlice *t_Slice = (cio_TimeSlice *)dfi_TSlice;
  if ( t_Slice->Write(fp, 1) != CIO::E_CIO_SUCCESS )
  {
    fclose(fp);
    return CIO::E_CIO_ERROR_WRITE_TIMESLICE;
  }

  return CIO::E_CIO_SUCCESS;

}


