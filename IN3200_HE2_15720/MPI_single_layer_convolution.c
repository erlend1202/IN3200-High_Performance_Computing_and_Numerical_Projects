#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>

/*
Performs sequential convolution on a MxN matrix.

Parameters
----------
    N : Number of columns in matrix.

    M : Number of columnsin matrix.

    K : Number of columnsand columns in kernel.

    input : A 2Darray that represents the matrix we want to convolve.

    output : A 2Darray that represents the resulting matrix after the convolution.

    kernel : A 2Darray that represents filter-matrix which we want to use.


*/
void single_layer_convolution (int M, int N, float **input, 
                               int K, float **kernel,
                               float **output)
{   
    int i,j,ii,jj;
    double temp;
    for (i=0; i<=M-K; i++)
        for (j=0; j<=N-K; j++) {
            temp = 0.0;
            for (ii=0; ii<K; ii++)
                for (jj=0; jj<K; jj++)
                    temp += input[i+ii][j+jj]*kernel[ii][jj];
            output[i][j] = temp;
    }
}



/*
Performs parallelized convolution on a MxN matrix using MPI.

Parameters
----------
    N : Number of columns in matrix.

    M : Number of columnsin matrix.

    K : Number of columnsand columns in kernel.

    input : A 2Darray that represents the matrix we want to convolve.

    output : A 2Darray that represents the resulting matrix after the convolution.

    kernel : A 2Darray that represents filter-matrix which we want to use.


*/
void MPI_single_layer_convolution (int M, int N, float **input, 
                                   int K, float **kernel,
                                   float **output)
{   
    int my_rank, num_procs;
    //finding rank and number of processors
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    // Calculate displacements and number of columns for each process.
    int *n_columns = malloc(num_procs*sizeof *n_columns);
    int *n_columns_gather = malloc(num_procs*sizeof *n_columns);

    // Displacement and number of elements sent when using scatter on input matrix.
    int *sendcounts = malloc(num_procs*sizeof *sendcounts);
    int *Sdispls = malloc(num_procs*sizeof *Sdispls);

    // Displacement when using gather on output matrix.
    int *Gdispls = malloc(num_procs*sizeof *Gdispls);

    /*
    the overlap is an important value to use when dividing convolution work.
    This is used so that each processor gets an overlap in columns, so that
    the convolution can work as intended.

    NOTE:
    Since we can assume that N and M are much larger than K, then it is
    safe to say that this method will spread the contents of input fairly
    evenly. If K were to be a lot larger, then the amount of overlap would be
    be greater, and the last process, which doesnt get the overlap, would have 
    a lot less work to do compared to the other processes.
    */
    int overlap = K - 1;
    //calculating amount of columns we pass to each processor
    int columns= N/num_procs;
    int remainder = N%num_procs;
    Sdispls[0] = 0;
    Gdispls[0] = 0;


    // Filling in the arrays which store the placement of elements, 
    // and the amount of elements passed for each row in input.
    for (int rank = 0; rank < num_procs-1; rank++) {
        n_columns[rank] = columns+ ((rank >= (num_procs - remainder)) ? 1:0) + overlap;
        n_columns_gather[rank] = n_columns[rank] - K + 1;
        sendcounts[rank] = n_columns[rank];
        Sdispls[rank+1] = Sdispls[rank] + sendcounts[rank] - overlap;
        Gdispls[rank+1] = Gdispls[rank] + n_columns_gather[rank];
    }
    //Setting values for the last processor, so that we avoid giving 
    // it overlap and going outside of the index values.
    n_columns[num_procs-1] = columns+ ((num_procs-1) >= (num_procs - remainder) ? 1:0);
    n_columns_gather[num_procs-1] = n_columns[num_procs-1] - K + 1;

    sendcounts[num_procs-1] = n_columns[num_procs-1];
    
    //allocating input and output 2Darray for the other ranks
    if (my_rank != 0) {
        input = (float**)malloc(M * sizeof(float*));
        for (int i = 0; i < M; i++){
            input[i] = (float*)malloc(n_columns[my_rank] * sizeof(float));
        }

        output = (float**)malloc((M-K+1) * sizeof(float*));
        for (int i = 0; i < (M-K+1); i++){
            output[i] = (float*)malloc((n_columns[my_rank]-K+1) * sizeof(float));
        }
    }

    //barrier to ensure that all the ranks have allocated their arrays before we scatter.
    MPI_Barrier(MPI_COMM_WORLD);
    //scattering each row of the 2Darray input.
    for (int i = 0; i < M; i++){
        MPI_Scatterv(input[i], sendcounts, Sdispls, MPI_FLOAT, input[i], n_columns[my_rank], MPI_FLOAT, 0, MPI_COMM_WORLD);
    }
    
    //barrier to ensure all ranks have gotten their values before we convolve.
    MPI_Barrier(MPI_COMM_WORLD);

    //calculating the convolution for each rank
    double temp;
    for (int i=0; i<=M-K; i++){
        for (int j=0; j<=n_columns[my_rank]-K; j++) {
            temp = 0.0;
            for (int ii=0; ii<K; ii++)
                for (int jj=0; jj<K; jj++)
                    temp += input[i+ii][j+jj]*kernel[ii][jj];
            output[i][j] = temp;
        }
    }    

    //barrier to ensure that all ranks have finished the convolution before we gather.
    MPI_Barrier(MPI_COMM_WORLD);
 
    //gathering the contents of output which we calculated above.
    for (int i = 0; i < M-K+1; i++){
        MPI_Gatherv(output[i], n_columns_gather[my_rank], MPI_FLOAT, output[i], n_columns_gather, Gdispls, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }
    
    //freeing the 2Darrays, input and output, for all ranks except 0.
    if (my_rank != 0){
        free(input[0]);
        free(output[0]);
        free(input);
        free(output);
    }

    //freeing the arrays we used to scatter and gather.
    free(n_columns);
    free(n_columns_gather);
    free(Sdispls);
    free(Gdispls);
    free(sendcounts);
}












