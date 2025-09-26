课程信息：高性能计算应用实践、2025秋  
实验名称：MPI并行编程 上课地点：T2612  
姓名：古权胜 学号：2024311286 班级：计信14

1、MPI DGEMM的实现和测试
 - 核心代码及说明
  ~~~MPI_Init(&argc, &argv);
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
  ~~~  

   >将矩阵数据结构定义为一维数组，方便进程间的数据发送与接收  
   >
   >在主线程中先将矩阵A按进程数量在行方向上均分，矩阵B保持不变，然后在主进程中将子矩阵A和矩阵B发送给各个子进程，同时完成主进程负责的计算部分  
   >
   >子进程分别接受数据，完成计算后结果存储在矩阵subC，将矩阵subC数据发送给主进程  
   >
   >主进程接收矩阵subC，将计算结果汇总复制在矩阵C中，完成计算  

 - 运行截图：输出需看到mpi_test_data.m中的`4*4`矩阵A、B、C的所有元素  
  ![](./image%20copy%202.png)  
  

2、碰到的问题及解决方法  
- **mpi无法正常工作**，猜测原因为 *"MPI 安装不完整或环境变量设置有问题"* ，尝试重装 *openmpi* 后，问题没有解决。**mpicc --version**输出为gcc版本说明环境变量设置有问题，通过以下命令解决  
 >**sudo update-alternatives --display mpi**  
 >检查当前的 MPI 替代方案  
 - **如何将结果传回root进程**，在子进程中将数据保存在一个小矩阵subC中，将subC发送给主进程，主进程在遍历subC复制到C中，存在效率低下的问题
