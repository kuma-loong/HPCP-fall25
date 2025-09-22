#include "stdio.h"
#include "stdlib.h"
#include "sys/time.h"
#include "time.h"
#include <cblas.h>

#define SIZE 16384

double A[SIZE * SIZE];
double B[SIZE * SIZE];
double C[SIZE * SIZE];

// 编译 gcc -o time_dgemm time_dgemm.c –lopenblas
// 运行 ./time_dgemm 1024
void dgemm_naive(int m, int n, int k, double alpha, double *A, int lda, double *B, int ldb, double beta, double *C, int ldc)
{
  int i, j, l;

  // 朴素矩阵乘法 C = alpha * A * B + beta * C
  // 这里的A、B、C是按列主序存储的
  for (j = 0; j < n; j++)
  {
    for (i = 0; i < m; i++)
    {
      // 先处理 beta * C 部分
      C[i + j * ldc] = beta * C[i + j * ldc];
      // 再计算 alpha * A * B 部分
      for (l = 0; l < k; l++)
      {
        C[i + j * ldc] += alpha * A[i + l * lda] * B[l + j * ldb];
      }
    }
  }
}


int main(int argc, char *argv[])
{
  /*
  if (argc < 2)
  {
    printf("Input Error\n");
    return 1;
  }
    */

  printf("test!\n");
  int i, m, n, k;
  m = n = k = SIZE;

  int sizeofa = m * k;
  int sizeofb = k * n;
  int sizeofc = m * n;
  int lda = m;
  int ldb = k;
  int ldc = m;

  double alpha = 1.2;
  double beta = 0.001;

  struct timeval start, finish;
  double duration;

  /*
  double *A = (double *)malloc(sizeof(double) * sizeofa);
  double *B = (double *)malloc(sizeof(double) * sizeofb);
  double *C = (double *)malloc(sizeof(double) * sizeofc);
  */
  srand((unsigned)time(NULL));

  for (i = 0; i < sizeofa; i++)
  {
    A[i] = i % 3 + 1; // (rand() % 100) / 100.0;
  }

  for (i = 0; i < sizeofb; i++)
  {
    B[i] = i % 3 + 1; //(rand()%100)/10.0;
  }

  for (i = 0; i < sizeofc; i++)
  {
    C[i] = 0.1;
  }

  printf("m=%d,n=%d,k=%d,alpha=%lf,beta=%lf,sizeofc=%d\n", m, n, k, alpha, beta, sizeofc);
  gettimeofday(&start, NULL);
  //dgemm_naive(m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
  gettimeofday(&finish, NULL);

  // 转成成秒数
  duration = (double)(finish.tv_sec - start.tv_sec) + (double)(finish.tv_usec - start.tv_usec) / 1.0e6;
  double gflops = 4.0 * m * n * k;
  gflops = gflops / duration * 1.0e-9;

  FILE *fp;
  fp = fopen("timeDGEMM.txt", "a"); // 追加写
  fprintf(fp, "%dx%dx%d\t%lf s\t%lf GFLOPS\n", m, n, k, duration, gflops);
  fclose(fp);

  /*
  free(A);
  free(B);
  free(C);
  */
  return 0;
}
