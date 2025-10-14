#include <stdio.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>
#include "defs.h"

/* Routine for computing C = A * B + C */
void MY_MMult(int m, int n, int k, double *a, int lda,
              double *b, int ldb,
              double *c, int ldc)
{
    cublasHandle_t handle;
    cublasCreate(&handle);

    // 分配GPU内存
    double *d_A, *d_B, *d_C;
    double alpha = 1.0;
    double beta = 1.0;
    
    cudaMalloc((void **)&d_A, m * k * sizeof(double));
    cudaMalloc((void **)&d_B, k * n * sizeof(double));
    cudaMalloc((void **)&d_C, m * n * sizeof(double));

    // copy数据到GPU
    cudaMemcpy(d_A, a, m * k * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, b, k * n * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_C, c, m * n * sizeof(double), cudaMemcpyHostToDevice);
 
    cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, 
                n, m, k, 
                &alpha, 
                d_B, ldb, 
                d_A, lda, 
                &beta, 
                d_C, ldc);
    cudaDeviceSynchronize();

    // copy结果回CPU
    cudaMemcpy(c, d_C, m * n * sizeof(double), cudaMemcpyDeviceToHost);

    // 释放GPU内存
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    cublasDestroy(handle);
}
