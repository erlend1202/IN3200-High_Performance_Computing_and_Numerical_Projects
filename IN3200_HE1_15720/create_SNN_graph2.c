#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

/*
Creates a SNN graph on the CRS format, using the row_ptr and col_idx
corresponding to the connectivity graph.

Parameters
----------
    row_ptr : Tells us which indexes correspond to which nodes in col_idx

    col_idx : A list of the edges between the nodes

    N : Number of nodes

*/

void create_SNN_graph2(int N, int *row_ptr, int *col_idx, int **SNN_val){
    int *temp_val;
    temp_val = (int*)malloc(row_ptr[N] * sizeof(int));

    size_t from_node, j, k, z;

    int count;
    int to_node;
    int mark;
   
    //looping over row_ptr, so that value i represents the i-th node
    for (from_node=0; from_node < N-1; from_node++){
        //looping over the elements in col_idx that belong to i-th node
        for (j=row_ptr[from_node]; j<row_ptr[from_node+1]; j++){
            //checking if the node has already gone through the loop
            //this reduces amount of iterations needed by half
            if (col_idx[j]>from_node){
                to_node = col_idx[j];
                count=0;
                
                k = row_ptr[from_node];
                z = row_ptr[to_node];
                /*looping over the indexes for from_node and to_node to check
                which nodes they have in common*/
                while (k < row_ptr[from_node+1] && z < row_ptr[to_node+1]){
                    //marking the index where from_node is in to_nodes list
                    if (col_idx[z] == from_node){
                            mark = z;
                    }
                    if (col_idx[k] < col_idx[z]) 
                        k++; 
                    else if (col_idx[z] < col_idx[k]) 
                        z++; 
                    else 
                    { 
                        count++; 
                        k++; 
                        z++; 
                    } 
                }
            
                temp_val[j] = count;
                temp_val[mark] = count;
            }
        }
    }

    *SNN_val = temp_val;
}

//OMP version
void create_SNN_graph2_OMP(int N, int *row_ptr, int *col_idx, int **SNN_val){
    int *temp_val;
    temp_val = (int*)malloc(row_ptr[N] * sizeof(int));

    size_t from_node, j, k, z;

    int count;
    int to_node;
    int mark;
   
    #ifdef _OPENMP
    #pragma omp parallel
    #pragma omp for private(j,k,z,count,mark,to_node) schedule(dynamic)
    #endif
    //looping over row_ptr, so that value i represents the i-th node
    for (from_node=0; from_node < N-1; from_node++){
        //looping over the elements in col_idx that belong to i-th node
        for (j=row_ptr[from_node]; j<row_ptr[from_node+1]; j++){
            //checking if the node has already gone through the loop
            //this reduces amount of iterations needed by half
            if (col_idx[j]>from_node){
                to_node = col_idx[j];
                count=0;

                k = row_ptr[from_node];
                z = row_ptr[to_node];
                /*looping over the indexes for from_node and to_node to check
                which nodes they have in common*/
                while (k < row_ptr[from_node+1] && z < row_ptr[to_node+1]){
                    //marking the index where from_node is in to_nodes list
                    if (col_idx[z] == from_node){
                            mark = z;
                    }
                    if (col_idx[k] < col_idx[z]) 
                        k++; 
                    else if (col_idx[z] < col_idx[k]) 
                        z++; 
                    else 
                    { 
                        count++; 
                        k++; 
                        z++; 
                    } 
                }
                temp_val[j] = count;
                temp_val[mark] = count;
            }
        }
    }    

    *SNN_val = temp_val;
}
