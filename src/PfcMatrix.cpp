#include <time.h>

#include "PfcMatrix.h"

using namespace std;


//******************************************************
// dgemm（行列と行列の乗算）のラッパー関数
void
PFC_dgemm( int M, int K,
           double alpha, double *A,
           double* B, double beta,
           double* C) {

  int N = M;

  char tran = 'N';

  dgemm_(&tran, &tran, &M, &N, &K, &alpha, A, &M, B, &K, &beta, C, &M);

}


//******************************************************
// dgemv（行列-ベクトル積）のラッパー関数
void
PFC_dgemv( int M, int N,
           double alpha, double *A,
           double *X,double beta,
           double *Y){


  char tran = 'N';
  int K = 1;

  dgemv_(&tran, &M, &N, &alpha, A, &M, X, &K, &beta, Y, &K);

}


//******************************************************
// 次元入れ替え （一次元の密データ） 
void PFC_matrix_Trans_d(double* dest, double* src, const int dest_row_size, const int dest_col_size)
{
  for(int i = 0; i < dest_row_size; i++) {
    for(int j = 0; j < dest_col_size; j++) {
      dest[j+i*dest_col_size] = src[i+j*dest_row_size];
    }
  }
}


//******************************************************
// 内積
double PFC_ddot(int N, double  *X, double  *Y){
  double result = 0.0;
  for(int i = 0; i < N; i++) {
    result = result + X[i]*Y[i];
  }
  return result;
}


//******************************************************
// dgeev（一般行列(n行n列の非対称行列)の固有値と左右の固有ベクトル）のラッパー関数
void PFC_dgeev(double *H, int n, double *Er, double *Evecs){

  double* Ei = new double[n];

  PFC_dgeev(H, n, Er, Ei, Evecs);

  delete []Ei;

}

//******************************************************
void PFC_dgeev(double *H, int n, double *Er, double *Ei, double *Evecs)
{
  char jobvl, jobvr;
  int lda,  ldvl, ldvr, lwork, info;
  double *a, *vl, *work;
  
  jobvl = 'N';
  jobvr = 'V';
  lda   = n;
  a = new double[n*lda];
  for( int i=0; i<n*lda; i++ ) {
    a[i] = H[i];
  }

  ldvl  = n;
  vl    = new double[n*n];
  ldvr  = n;
  work  = new double[4*n];
  lwork = 4*n;


  dgeev_(&jobvl, &jobvr, &n, a, &lda, Er, Ei, vl,
     &ldvl, Evecs, &ldvr, work, &lwork, &info);

  PFC_d_sort(Er, Ei, Evecs, n);

  delete []a;
  delete []vl;
  delete []work;

}


//******************************************************
// 絶対値の小さい順（昇順）に並べる
//     PFClibでは N=2～4 : quick sort等への置き換えはしない
void PFC_d_sort(double *Er, double *Ei, double *Evecs, int N)
{
  double temp, *E2;
  int i, j, k;
  
  E2 = new double[N];
  for (i=0; i<N; i++) {
    E2[i] = Er[i]*Er[i]+Ei[i]*Ei[i];
  }
  
  for (j=0; j<N-1; j++) {
    for (i=j+1; i<N; i++) {
      if (fabs(E2[j])<fabs(E2[i])) {
        temp = E2[j]; E2[j] = E2[i]; E2[i] = temp;
        temp = Er[j]; Er[j] = Er[i]; Er[i] = temp;
        temp = Ei[j]; Ei[j] = Ei[i]; Ei[i] = temp;
      
        for (k=0; k<N; k++) {
          temp = Evecs[N*j+k];
          Evecs[N*j+k] = Evecs[N*i+k];
          Evecs[N*i+k] = temp;
        }
      }
    }
  }

  delete []E2;
}

