#include <stdio.h>
#include <stdlib.h>
#include "sys/time.h"
#include "time.h"
#include <unistd.h>
#include <sys/wait.h>

void dgemm(int m, int n, int k, int beta,
          double A[][k], double B[][n], double C[][n]){
    for(int i=0; i< m;i ++){    //C[i] 
        for(int j=0; j< n; j++){  //C[i][j]
            C[i][j] = beta*C[i][j];
            for(int p=0; p< k; p++){  
                C[i][j] += A[i][p]*B[p][j]; 
             }
        }
    }
}

void printf_matrix(int row, int col, double matrix[row][col] ){
  for(int i=0; i<row; i++){
    for(int j=0; j<col;j++){
        printf("%lf ", matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n\n");
}


int main(int argc, char *argv[]){
  if (argc < 2)
  {
    printf("Input Error\n");
    return 1;
  }

  int i, m, n, k;
  m = n = k = atoi(argv[1]);

  int sizeofa = m * k;
  int sizeofb = k * n;
  int sizeofc = m * n;
  int lda = m;
  int ldb = k;
  int ldc = m;

  double alpha = 1.2;
  double beta = 0.001;

  double *A = (double *)malloc(sizeof(double) * sizeofa);
  double *B = (double *)malloc(sizeof(double) * sizeofb);
  double *C = (double *)malloc(sizeof(double) * sizeofc);
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

  int wc = fork();
  //int wc = 1;


  if (wc < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (wc == 0) {
    printf("child process pid = %d\n", (int)getpid());
    dgemm(m, n, k, 1, (double (*)[k])A, (double (*)[n])B, (double (*)[n])C);
    //printf_matrix(m, n, (double (*)[n])C);
    exit(0);
  } else {
      printf("parent process pid = %d\n", (int)getpid());
      dgemm(m, n, k, 1, (double (*)[k])A, (double (*)[n])B, (double (*)[n])C);
      wait(NULL);
      // printf_matrix(m, n, (double (*)[n])C);
  }

}