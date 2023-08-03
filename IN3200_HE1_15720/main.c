#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "read_graph_from_file1.h"
#include "read_graph_from_file2.h"
#include "create_SNN_graph1.h"
#include "create_SNN_graph2.h"
#include "tests.h"

int main(){
    int nodes;
    char **table2D;

    double start; 
    double end; 

    int *row;
    int *col;
    int **SNN_table;
    int **SNN_table_OMP;
    int *SNN_val;
    int *SNN_val_OMP;

    start = omp_get_wtime();
    read_graph_from_file1("facebook_combined.txt", &nodes, &table2D);
    end = omp_get_wtime(); 
    printf("Work 2Dtable took %f seconds\n", end - start);

    start = omp_get_wtime(); 
    read_graph_from_file2("facebook_combined.txt", &nodes, &row, &col);
    end = omp_get_wtime(); 
    printf("Work CRS_FORMAT took %f seconds\n", end - start);

    start = omp_get_wtime(); 
    create_SNN_graph1(nodes, table2D, &SNN_table);
    end = omp_get_wtime(); 
    printf("Work SNN_2Dtable took %f seconds\n", end - start);

    start = omp_get_wtime(); 
    create_SNN_graph1_OMP(nodes, table2D, &SNN_table_OMP);
    end = omp_get_wtime(); 
    printf("Work SNN_2Dtable with OMP took %f seconds\n", end - start);
    
    start = omp_get_wtime();
    create_SNN_graph2(nodes, row, col, &SNN_val);
    end = omp_get_wtime(); 
    printf("Work SNN_CRS_FORMAT took %f seconds\n", end - start);

    start = omp_get_wtime();
    create_SNN_graph2_OMP(nodes, row, col, &SNN_val_OMP);
    end = omp_get_wtime(); 
    printf("Work SNN_CRS_FORMAT with OMP took %f seconds\n", end - start);
    
    /*testing if the CRS format calculations correspond to 
    the 2Dtable format calculations. This ensures if the test passes, 
    that both the CRS and 2Dtable methods are correct*/
    test_table2D_equals_row_col(table2D, row, col, nodes);
    test_SNN_table_equals_SNN_val(SNN_table, row, col, SNN_val, nodes);
    test_SNN_table_equals_SNN_val(SNN_table_OMP, row, col, SNN_val_OMP, nodes);

    free(row);
    free(col);

    free(SNN_val);

    free(SNN_val_OMP);


    free(table2D[0]);
    free(table2D);

    free(SNN_table[0]);
    free(SNN_table);

    free(SNN_table_OMP[0]);
    free(SNN_table_OMP);
    return 0;
}

/*
Output when run:

Work 2Dtable took 0.066099 seconds
Work CRS_FORMAT took 0.041769 seconds
Work SNN_2Dtable took 1.409660 seconds
Work SNN_2Dtable with OMP took 0.459451 seconds
Work SNN_CRS_FORMAT took 0.106755 seconds
Work SNN_CRS_FORMAT with OMP took 0.030815 seconds
*/