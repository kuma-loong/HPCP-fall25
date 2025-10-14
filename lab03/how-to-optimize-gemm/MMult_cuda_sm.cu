#include <stdio.h>
#include <cuda_runtime.h>
#include "defs.h"

#define TILE_SIZE 16

// GPU Kernel：使用共享内存的矩阵乘法
__global__ void MMultKernelShared(int m, int n, int k,
                                  double *A, int lda,
                                  double *B, int ldb,
                                  double *C, int ldc)
{
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // 当前线程对应全局矩阵 C 的行列
    int row = blockIdx.y * blockDim.y + ty;
    int col = blockIdx.x * blockDim.x + tx;

    double result = 0.0;

    // 声明共享内存 tile
    __shared__ double As[TILE_SIZE][TILE_SIZE];
    __shared__ double Bs[TILE_SIZE][TILE_SIZE];

    // 按 k 方向分块处理
    for (int t = 0; t < (k + TILE_SIZE - 1) / TILE_SIZE; t++)
    {
        // 每个线程从全局内存加载 A、B 的一部分到共享内存
        int aCol = t * TILE_SIZE + tx;
        int bRow = t * TILE_SIZE + ty;

        // 边界检查 (矩阵B转置以避免Bank冲突)
        As[ty][tx] = (row < m && aCol < k) ? A[row * lda + aCol] : 0.0;
        Bs[tx][ty] = (bRow < k && col < n) ? B[bRow * ldb + col] : 0.0;

        // 确保整个 tile 加载完成
        __syncthreads();

        #pragma unroll 8        //循环展开
        for (int i = 0; i < TILE_SIZE; i++) {
            result += As[ty][i] * Bs[tx][i];
        }

        // 确保本轮计算完成，下一轮可以覆盖共享内存
        __syncthreads();
    }

    if (row < m && col < n) {
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
        cudaMemcpy(d_C, c, m * n * sizeof(double), cudaMemcpyHostToDevice);

        dim3 block(TILE_SIZE, TILE_SIZE);
        dim3 grid((n + block.x - 1) / block.x, (m + block.y - 1) / block.y);
        MMultKernelShared<<<grid, block>>>(m, n, k, d_A, lda, d_B, ldb, d_C, ldc);
        cudaDeviceSynchronize();

        // copy结果回CPU
        cudaMemcpy(c, d_C, m * n * sizeof(double), cudaMemcpyDeviceToHost);

        // 释放GPU内存
        cudaFree(d_A);
        cudaFree(d_B);
        cudaFree(d_C);
}
