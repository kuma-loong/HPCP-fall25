#include <stdio.h>
#include <cuda_runtime.h>
#include "defs.h"

__global__ void MMultKernel(int m, int n, int k, double *A, int lda,
                            double *B, int ldb,
                            double *C, int ldc)
{
    // 计算当前线程的行和列索引
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < m && col < n)
    {
        double result = 0.0;

        for (int i = 0; i < k; i++)
        {
            result += A[row * lda + i] * B[i * ldb + col];
        }

        C[row * ldc + col] += result;
    }

}
    /* Routine for computing C = A * B + C */
void MY_MMult(int m, int n, int k, double *a, int lda,
              double *b, int ldb,
              double *c, int ldc)
{        
    // 分配GPU内存
    double *d_A, *d_B, *d_C;
    cudaMalloc((void **)&d_A, m * k * sizeof(double));
    cudaMalloc((void **)&d_B, k * n * sizeof(double));
    cudaMalloc((void **)&d_C, m * n * sizeof(double));

    // copy数据到GPU
    cudaMemcpy(d_A, a, m * k * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, b, k * n * sizeof(double), cudaMemcpyHostToDevice);

    dim3 block(16, 16);
    dim3 grid((n + block.x - 1) / block.x, (m + block.y - 1) / block.y);
    MMultKernel<<<grid, block>>>(m, n, k, d_A, lda, d_B, ldb, d_C, ldc);
    cudaDeviceSynchronize();

    // copy结果回CPU
    cudaMemcpy(c, d_C, m * n * sizeof(double), cudaMemcpyDeviceToHost);

    // 释放GPU内存
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}

