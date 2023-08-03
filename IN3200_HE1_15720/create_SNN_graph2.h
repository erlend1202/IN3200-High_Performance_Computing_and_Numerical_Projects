#ifndef CREATE_SNN_GRAPH2
#define CREATE_SNN_GRAPH2

void create_SNN_graph2(int N, int *row_ptr, int *col_idx, int **SNN_val);
void create_SNN_graph2_OMP(int N, int *row_ptr, int *col_idx, int **SNN_val);

#endif