#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "defs.h"
#define BLOCK_SIZE 8

typedef struct {
    int row_start, row_end;
    int col_start, col_end;
    int k;
    double *a, *b, *c;
    int lda, ldb, ldc;
} thread_arg_t;

void *mythread(void *arg) {
    thread_arg_t *args = (thread_arg_t *)arg;
    int row_start = args->row_start;
    int row_end = args->row_end;
    int col_start = args->col_start;
    int col_end = args->col_end;
    int lda = args->lda;
    int ldb = args->ldb;
    int ldc = args->ldc;
    double *a = args->a;
    double *b = args->b;
    double *c = args->c;
    int k = args->k;

    for (int ii = row_start; ii < row_end; ii += BLOCK_SIZE)
    {
        int i_max = (ii + BLOCK_SIZE < row_end) ? ii + BLOCK_SIZE : row_end;

        for (int jj = col_start; jj < col_end; jj += BLOCK_SIZE)
        {
            int j_max = (jj + BLOCK_SIZE < col_end) ? jj + BLOCK_SIZE : col_end;

            for (int pp = 0; pp < k; pp += BLOCK_SIZE)
            {
                int p_max = (pp + BLOCK_SIZE < k) ? pp + BLOCK_SIZE : k;

                // 小块乘法
                for (int i = ii; i < i_max; i++)
                {
                    for (int j = jj; j < j_max; j++)
                    {
                        double sum = C(i, j); // 缓存结果
                        for (int p = pp; p < p_max; p++)
                        {
                            sum += A(i, p) * B(p, j);
                        }
                        C(i, j) = sum;
                    }
                }
            }
        }
    }

    return NULL;
}

void MY_MMult(int m, int n, int k, double *a, int lda,
              double *b, int ldb,
              double *c, int ldc) 
{
    int num_threads = (int) sysconf(_SC_NPROCESSORS_ONLN); // 线程数量
    //int num_threads = 4;

    pthread_t threads[num_threads];
    thread_arg_t args[num_threads];

    int rows_per_thread = m / num_threads;
    for (int i = 0; i < num_threads; i++) {
       args[i].row_start = i * rows_per_thread;
       args[i].row_end = (i == num_threads - 1) ? m : (i + 1) * rows_per_thread;
       args[i].col_start = 0;
       args[i].col_end = n;
       args[i].lda = lda;
       args[i].ldb = ldb;
       args[i].ldc = ldc;
       args[i].k = k; 
       args[i].a = a;
       args[i].b = b;
       args[i].c = c;
       pthread_create(&threads[i], NULL, mythread, (void *)&args[i]);                                      
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}