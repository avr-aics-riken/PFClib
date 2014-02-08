#ifndef _PFC_MATRIX_H
#define _PFC_MATRIX_H

#include <math.h>
using namespace std;

extern "C" void dgemm_(char *transa, char *transb, int *m, int *n, int *k, 
               double *alpha, double *A, int *ldA, double *B, int *ldB, 
               double *beta , double *C, int *ldC); 
extern "C" void dgemv_(char *trans, int *m, int *n, double *alpha, double *A, int *ldA, 
               double *x, int *incx, double *beta, double *y, int *incy ); 
extern "C" int dgeev_(char * jobvl, char *jobvr, int *n, double *a, int *lda, double *wr, double *wi, double *vl, int *ldvl, double * vr, int *ldvr, double *work, int *lwork, int *info); 


  /**
   * @brief dgemm（行列と行列の乗算）のラッパー関数
   * @param[in]   M
   * @param[in]   K
   * @param[in]   alpha
   * @param[in]   A(*,M)
   * @param[in]   B(*,K)
   * @param[in]   beta
   * @param[out]  C(*,M)
   * @return なし
   */
void PFC_dgemm(int M, int K,
           double alpha, double *A,
           double* B, double beta,
           double* C);

  /**
   * @brief dgemv（行列-ベクトル積）のラッパー関数
   * @param[in]   M
   * @param[in]   N
   * @param[in]   alpha
   * @param[in]   A(M,N)
   * @param[in]   X(N)
   * @param[in]   beta
   * @param[out]  Y(N)
   * @return なし
   */
void PFC_dgemv(int M, int N,
           double alpha, double *A,
           double *X,double beta,
           double *Y);

  /**
   * @brief 次元入れ替え （一次元の密データ）
   * @param[out]  dest      転送先
   * @param[in]   src       転送元（ソース）
   * @param[in]   dest_row  転送先行数
   * @param[in]   dest_col  転送先列数
   * @return なし
   */
void PFC_matrix_Trans_d(double* dest, double* src, const int dest_row_size, const int dest_col_size);

  /**
   * @brief 内積
   * @param[in]  N      サイズ
   * @param[in]  X      X[N]
   * @param[in]  Y      Y[N]
   * @return  内積値
   */
double PFC_ddot(int N, double  *X, double  *Y);

  /**
   * @brief dgeev（一般行列(n行n列の非対称行列)の固有値と左右の固有ベクトル）のラッパー関数
   * @param[in]    H[n][n]
   * @param[in]    n
   * @param[out]   Er[n]
   * @param[out]   Evecs[n][n]
   * @return なし
   */
void PFC_dgeev(double *H, int n, double *Er, double *Evecs);
void PFC_dgeev(double *H, int n, double *Er, double *Ei, double *Evecs);



  /**
   * @brief ソート   （Er*Er+Ei*Ei)の絶対値の小さい順（昇順）に並べる
   * @param[inout]   Er[N]
   * @param[inout]   Ei[N]
   * @param[inout]   Evecs[N][N]
   * @param[in]      N
   * @return なし
   */
void PFC_d_sort(double *Er, double *Ei, double *Evecs, int N);

#endif  // _PFC_MATRIX_H

