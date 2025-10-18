#include <stdio.h>
#include <cuda_runtime.h>
#include "defs.h"

#define TILE_SIZE 16

// GPU Kernel：使用共享内存的矩阵乘法
__global__ void MMultKernelShared(int m, int n, int k,
                                  float *A, int lda,
                                  float *B, int ldb,
                                  float *C, int ldc)
{
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // 当前线程对应全局矩阵 C 的行列
    int row = blockIdx.y * blockDim.y + ty;
    int col = blockIdx.x * blockDim.x + tx;

    float result = 0.0f;

    // 声明共享内存 tile
    __shared__ float As[TILE_SIZE][TILE_SIZE];
    __shared__ float Bs[TILE_SIZE][TILE_SIZE];

    // 按 k 方向分块处理
    #pragma unroll
    for (int t = 0; t < (k + TILE_SIZE - 1) / TILE_SIZE; t++)
    {
        // 每个线程从全局内存加载 A、B 的一部分到共享内存
        int aCol = t * TILE_SIZE + tx;
        int bRow = t * TILE_SIZE + ty;

        // 边界检查 (矩阵B转置以避免Bank冲突)
        As[ty][tx] = (row < m && aCol < k) ? A[row * lda + aCol] : 0.0f;
        Bs[tx][ty] = (bRow < k && col < n) ? B[bRow * ldb + col] : 0.0f;

        // 确保整个 tile 加载完成
        __syncthreads();

        #pragma unroll        //循环展开
        for (int i = 0; i < TILE_SIZE; i++)
            result += As[ty][i] * Bs[tx][i];

        // 确保本轮计算完成，下一轮可以覆盖共享内存
        __syncthreads();
    }

    if (row < m && col < n)
    {
        C[row * ldc + col] = result;
    }
}

// fp64 转 fp32
__global__ void double_to_float_kernel(int size_1, const double *in_1, float *out_1,
                                       int size_2, const double *in_2, float *out_2)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < size_1)
    {
        out_1[idx] = static_cast<float>(in_1[idx]);
    }
    if (idx < size_2)
    {
        out_2[idx] = static_cast<float>(in_2[idx]);
    }
}

//fp32 转 fp64 并且完成加法操作
__global__ void float_to_double_kernel(int size, const float *in, double *out)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < size)
    {
        out[idx] += static_cast<double>(in[idx]);
    }
}

/* Routine for computing C = A * B + C */
void MY_MMult(int m, int n, int k, 
              double *a, int lda,
              double *b, int ldb,
              double *c, int ldc)
{
    // 分配FP64设备内存 (用于与主机交互)
    double *d_A_fp64, *d_B_fp64, *d_C_fp64;
    cudaMalloc((void **)&d_A_fp64, m * k * sizeof(double));
    cudaMalloc((void **)&d_B_fp64, k * n * sizeof(double));
    cudaMalloc((void **)&d_C_fp64, m * n * sizeof(double));

    // 分配FP32设备内存 (用于核心计算)
    float *d_A_fp32, *d_B_fp32, *d_C_fp32;
    cudaMalloc((void **)&d_A_fp32, m * k * sizeof(float));
    cudaMalloc((void **)&d_B_fp32, k * n * sizeof(float));
    cudaMalloc((void **)&d_C_fp32, m * n * sizeof(float));

    // 将主机数据拷贝到FP64设备内存
    cudaMemcpy(d_A_fp64, a, m * k * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B_fp64, b, k * n * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_C_fp64, c, m * n * sizeof(double), cudaMemcpyHostToDevice);

    // double → float
    int threads = 256;
    int blocksA = (m * k + threads - 1) / threads;
    int blocksB = (k * n + threads - 1) / threads;
    int blocksC = (m * n + threads - 1) / threads;

    int blocks = max(blocksA, blocksB);

    double_to_float_kernel<<<blocks, threads>>>(m * k, d_A_fp64, d_A_fp32,
                                                k * n, d_B_fp64, d_B_fp32);
    cudaDeviceSynchronize();

    // 核心计算
    dim3 block(TILE_SIZE, TILE_SIZE);
    dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (m + TILE_SIZE - 1) / TILE_SIZE);
    MMultKernelShared<<<grid, block>>>(m, n, k, d_A_fp32, lda, d_B_fp32, ldb, d_C_fp32, ldc);
    cudaDeviceSynchronize();

    // float → double
    float_to_double_kernel<<<blocksC, threads>>>(m * n, d_C_fp32, d_C_fp64);
    cudaDeviceSynchronize();

    // copy结果回CPU
    cudaMemcpy(c, d_C_fp64, m * n * sizeof(double), cudaMemcpyDeviceToHost);

    // 释放GPU内存
    cudaFree(d_A_fp64);
    cudaFree(d_B_fp64);
    cudaFree(d_C_fp64);
    cudaFree(d_A_fp32);
    cudaFree(d_B_fp32);
    cudaFree(d_C_fp32);
}