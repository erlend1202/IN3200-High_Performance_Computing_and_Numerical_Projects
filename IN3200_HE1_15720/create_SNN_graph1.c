#include <stdlib.h>
#include <stdio.h>
#include <omp.h>


/*
Creates a SNN graph in 2Dtable format.

Parameters
----------
    N : Number of nodes

    table2D : A 2Darray which stores the edges between every node.
*/

void create_SNN_graph1(int N, char **table2D, int ***SNN_table){    
    int **temp;
    temp = (int**)malloc(N * sizeof(int*));

    size_t i, j, k;

    //allocating and setting values to 0
    for (i=0; i<N; i++){
        temp[i] = (int*)malloc(N * sizeof(int));
        for (j=0; j<N; j++){
            temp[i][j] = 0;
        }
    }

    //looping over rows (i) and collumns (j) in 2Dtable
    int count;
    for (i=0; i<N-1; i++){
        /*setting the j indixes such that we only focus on the right 
        side of the diagonal*/
        for (j=i+1; j<N; j++){
            /*if node i is connected to node j, then we loop over and find 
            similar nodes they share */
            if (table2D[i][j] == '1'){
                count = 0;
                for (k=0; k<N; k++){
                    if (table2D[i][k] == table2D[j][k] && table2D[i][k]=='1'){
                        count += 1;
                    }
                }
                temp[i][j] = count;
                temp[j][i] = count; 
            }
        }
    }
    *SNN_table = temp; 
}

//openMP parallelization version
void create_SNN_graph1_OMP(int N, char **table2D, int ***SNN_table){    
    int **temp;
    temp = (int**)malloc(N * sizeof(int*));
    
    size_t i, j, k;

    for (i=0; i<N; i++){
        temp[i] = (int*)malloc(N * sizeof(int));
        for (j=0; j<N; j++){
            temp[i][j] = 0;
        }
    }

    int count;
    /*parallelizing the code, with j,k, and count set to private
    to avoid multiple threads working on the same indexes or 
    setting the values wrong*/
    #ifdef _OPENMP
    #pragma omp parallel
    #pragma omp for private(j,k,count) schedule(dynamic)
    #endif
    //looping over rows (i) and collumns (j) in 2Dtable
    for (i=0; i<N-1; i++){
        /*setting the j indixes such that we only focus on the right 
        side of the diagonal*/
        for (j=i+1; j<N; j++){
            /*if node i is connected to node j, then we loop over and find 
            similar nodes they share */
            if (table2D[i][j] == '1'){
                count = 0;
                for (k=0; k<N; k++){
                    if (table2D[i][k] == table2D[j][k] && table2D[i][k]=='1'){
                        count += 1;
                    }
                }
                temp[i][j] = count;
                temp[j][i] = count; 
            }
        }
    }

    *SNN_table = temp; 
}


