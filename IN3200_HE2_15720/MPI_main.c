#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include "MPI_single_layer_convolution.h"

//mpicc MPI_single_layer_convolution.c -o run
//mpirun --mca btl_vader_single_copy_mechanism none -np 4 ./run N M K

/*
mpicc MPI_single_layer_convolution.c -o run
mpirun --mca btl_vader_single_copy_mechanism none -np 4 ./run 2000 2000 15
example of output using these parameters:

time spent using mpi: 468 ms
time spent using sequential: 1625 ms
*/

void single_layer_convolution (int M, int N, float **input, 
                               int K, float **kernel,
                               float **output);
void MPI_single_layer_convolution (int M, int N, float **input, 
                                   int K, float **kernel,
                                   float **output);


int main(int argc, char *argv[]) {
    int my_rank, num_procs;
    int M=0, N=0, K=0;
    float **input = NULL, **output = NULL, **kernel=NULL;
    
    //initializing MPI, and setting values to rank and number of processors
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //Doing some work on rank 0, which will be the root.
    if (my_rank == 0) {
        //setting values for N,M,K from commandline
        if (argc>3) {
            N = atoi(argv[1]);
            M = atoi(argv[2]);
            K = atoi(argv[3]);
        } else {
            printf("not enough input arguments given, N, M, K set to: %d, %d, %d \n", num_procs*5 + 1, num_procs*5 + 1, 3);
            N = num_procs*5 + 1;
            M = num_procs*5 + 1;
            K = 3;
        }

        
        // tests to stop the program from running if the 
        // values we have set wont work when running the program.
        
        if (N < K || M < K) {
            printf("N or M are smaller than K, which is not acceptable values");
            exit(1);
        }

        else if (N/num_procs + N%num_procs < K || N/num_procs < 1) {
            printf("Number of processors is too large with given values of N and K");
            exit(1);
        }

        // allocating and filling in values for
        // the 2Darrays we want for later use
        input = (float**)malloc(M * sizeof(float*));
        output = (float**)malloc((M-K+1) * sizeof(float*));
        kernel = (float**)malloc(K * sizeof(float*));

        for (int i=0; i < M; i++) {
            input[i] = (float*)malloc(N * sizeof(float));
            for (int j=0; j<N; j++){
                input[i][j] = j + 1 + i;
            }
        }

        for (int i = 0; i < M-K+1; i++){
            output[i] = (float*)malloc((N-K+1) * sizeof(float));  
        }

        for (int i = 0; i < K; i++){
            kernel[i] = (float*)malloc(K * sizeof(float));
            for (int j = 1; j < K-1; j++) {
                kernel[i][j] = 0;
            }
            kernel[i][0] = 1;
            kernel[i][K-1] = -1;
        }
    }


    //Broadcasting the values for N,M,K to the other ranks
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //barrier to ensure that all ranks got their K values before allocating K*K kernel.
    MPI_Barrier(MPI_COMM_WORLD);

    //allocating space for kernel for all ranks except 0.
    if (my_rank > 0) {
        kernel = (float**)malloc(K * sizeof(float*));
        for (int i = 0; i < K; i++){
            kernel[i] = (float*)malloc(K * sizeof(float));
        }
    }

    //barrier to ensure all ranks have finished allocating before broadcasting kernel.
    MPI_Barrier(MPI_COMM_WORLD);

    //broadcasting each row of kernel from 0 to the other ranks.
    for (int i = 0; i < K; i++){
        MPI_Bcast(kernel[i], K, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }

    clock_t start_mpi, end_mpi, start_sequential, end_sequential;
    clock_t total_mpi, total_sequential;

    //calling the function for MPI convolution, and timing it.
    start_mpi = clock();
    MPI_single_layer_convolution(M,N,input,K,kernel,output);
    end_mpi = clock();

    if (my_rank == 0) {
        // allocating a temporary 2Darray to use for comparing results of
        // sequential convolution and MPI parallelized convolution. 
        float **temp;
        temp = (float**)malloc((M-K+1) * sizeof(float*));
        for (int i = 0; i < M-K+1; i++){
            temp[i] = (float*)malloc((N-K+1) * sizeof(float));
        }

        //calling function for sequential convolution, and timing it.
        start_sequential = clock();
        single_layer_convolution(M, N, input, K, kernel, temp);
        end_sequential = clock();

        // checking if each element in the results from sequential 
        // convolution and MPI convolution are the same.
        for (size_t i=0; i < M-K+1; i++) {
            for (size_t j=0; j<N-K+1; j++){
                if (temp[i][j] != output[i][j]){
                    printf("MPI and sequential method are not same");
                    exit(1);
                }
            }
        }

        //printing the time it took for both implementations
        total_mpi = (double)(end_mpi - start_mpi) / CLOCKS_PER_SEC *1000;
        total_sequential = (double)(end_sequential - start_sequential) / CLOCKS_PER_SEC * 1000;
        printf("time spent using mpi: %ld ms\n", total_mpi);
        printf("time spent using sequential: %ld ms\n", total_sequential);

        // freeing the 2Darrays input, output, and temp, which are 
        // currently only allocated on rank 0.
        free(input[0]);
        free(output[0]);
        free(temp[0]);

        free(temp);
        free(input);
        free(output);
    }

    //barrier so that we dont free kernel before rank 0 is done using it.
    MPI_Barrier(MPI_COMM_WORLD);

    //freeing kernel on all ranks.
    free(kernel[0]);
    free(kernel);

    MPI_Finalize();
    return 0;
}