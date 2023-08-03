#ifndef MPI_SINGLE_LAYER_CONVOLUTION
#define MPI_SINGLE_LAYER_CONVOLUTION

void single_layer_convolution (int M, int N, float **input, 
                               int K, float **kernel,
                               float **output);
void MPI_single_layer_convolution (int M, int N, float **input, 
                                   int K, float **kernel,
                                   float **output);
                                   
#endif