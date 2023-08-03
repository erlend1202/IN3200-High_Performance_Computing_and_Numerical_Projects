#ifndef TESTS
#define TESTS

// Function that tests if read_graph_from_file1() and read_graph_from_file2() gives
// equivalent results. 
void test_table2D_equals_row_col(char **table2D, int *row_ptr, int *col_idx, int N){
    for (size_t i = 0; i < N; i++){
        for (size_t j = row_ptr[i]; j < row_ptr[i+1]; j++){
            if (table2D[i][col_idx[j]] != '1'){
                printf("read_graph_from_file1() and read_graph_from_file2() are not equivalent!\n");
                exit(1);
            }
        }
        
    }
}

// Function that tests if create_SNN_graph1() and create_SNN_graph2() gives
// equivalent results. 
void test_SNN_table_equals_SNN_val(int **SNN_table, int *row_ptr, int *col_idx, int *SNN_val, int N){
    for (size_t i = 0; i < N; i++){
        for (size_t j = row_ptr[i]; j < row_ptr[i+1]; j++){
            if (SNN_table[i][col_idx[j]] != SNN_val[j]){
                printf("create_SNN_graph1() and create_SNN_graph2() are not equivalent!\n");
                exit(1);
            }
        }
        
    }
}

#endif
