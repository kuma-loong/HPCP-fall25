#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>

#define N 4 


void print_matrix(const double *M, int n, const char *name) {
    printf("%s = \n", name);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%.6e ", M[i * n + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int numprocs, myid;
    MPI_Status status;

    double A[N * N] = {
        -1.000000e+00, -2.707955e-01,  5.350056e-02,  8.634630e-01,
        -9.980292e-01, -8.173388e-01, -9.113315e-02,  1.361192e-01,
        -9.167380e-01, -8.154047e-01, -5.336431e-01,  1.121887e-01,
        -6.467147e-01, -2.556555e-02,  6.625836e-01, -8.983362e-01
    };

    double B[N * N] = {
        5.341023e-01,  7.519617e-01,  6.208589e-01, -8.464509e-01,
        -9.621704e-01, 6.311373e-02, -6.231595e-01,  6.305478e-01,
        -4.952805e-01, 8.405219e-01,  7.726289e-01,  9.697820e-01,
        -4.036057e-01, 3.086230e-02,  1.412280e-01, -7.632966e-01
    };

    double C[N * N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    const int rows_per_proc = N / numprocs;
    const int extra_rows = N % numprocs;

    if (myid == 0) {
        printf("Number of processes: %d\n", numprocs);
        // 主进程初始化矩阵 C
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                C[i * N + j] = 0.0;
            }
        }

        // 将矩阵块分发给子进程
        for (int p = 1; p < numprocs; p++) {
            const int start_row = p * rows_per_proc;
            MPI_Send(&A[start_row * N], rows_per_proc * N, MPI_DOUBLE, p, 0, MPI_COMM_WORLD);
            MPI_Send(B, N * N, MPI_DOUBLE, p, 0, MPI_COMM_WORLD);
        }

        // 主进程计算自己部分
        for (int i = 0; i < rows_per_proc; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    C[i * N + j] += A[i * N + k] * B[k * N + j];
                }
            }
        }

        // 接收子进程计算结果
        for (int p = 1; p < numprocs; p++) {
            double subC[rows_per_proc * N];
            MPI_Recv(subC, rows_per_proc * N, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, &status);
            const int start_row = p * rows_per_proc;
            
            for (int i = 0; i < rows_per_proc; i++) {
                for (int j = 0; j < N; j++) {
                    C[(start_row + i) * N + j] = subC[i * N + j];
                }
            }
        }

        // 打印矩阵
        print_matrix(A, N, "A");
        print_matrix(B, N, "B");
        print_matrix(C, N, "C");
        
    } else {
        // 子进程接收 A 块和整个 B
        double subA[rows_per_proc * N];
        double subC[rows_per_proc * N];
        for (int i = 0; i < rows_per_proc * N; i++) {// 初始化为零
            subC[i] = 0.0;
        }
        
        MPI_Recv(subA, rows_per_proc * N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(B, N * N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

        // 计算 C 块
        for (int i = 0; i < rows_per_proc; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    subC[i * N + j] += subA[i * N + k] * B[k * N + j];
                }
            }
        }

        
        MPI_Send(subC, rows_per_proc * N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
