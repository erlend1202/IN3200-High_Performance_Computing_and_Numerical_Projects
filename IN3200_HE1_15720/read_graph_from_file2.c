#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

/*
Swaps values between two given elements a and b.
*/
void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*
Creates a CRS_graph format of node connectivity, which consists 
of row_ptr and col_idx which are 1D arrays to the nodes.
Also finds the number of nodes.

Parameters
----------
    filename: Filename of the .txt file we want to open and read the connectivity graph from.

Notes
-----
Allocates arrays inside function and then sets the variables row_ptr and col_idx as 
those arrays, so that they are stored outside the function call.
Also sets N as number of nodes which we find.
*/
void read_graph_from_file2(char *filename, int *N, int **row_ptr, int **col_idx){

    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failure to open file %s\n", filename);
        exit(0);
    }
    size_t i = 0;
    char buffer[100];
    for (i=0; i<3; i++){
        fgets(buffer, 100, file);
    }

    int nodes, edges;
    sscanf(buffer, "# Nodes: %d Edges: %d ", &nodes, &edges);
    fgets(buffer, 100, file);

    int n = nodes;

    int *temp_row;
    int *temp_col;

    temp_row = (int*)malloc((n + 1) * sizeof(int));
    temp_col = (int*)malloc(2*edges * sizeof(int));

    //reading through the file and storing amount of a given node
    int from_node, to_node;
    for (i=0; i < edges; i++) {
        fscanf(file, "%d %d", &from_node, &to_node);
            if (from_node == to_node){
                i--;
                continue;
            }
            else if (from_node < 0 || from_node >= nodes || to_node < 0 || to_node >= nodes){
                i--;
                continue;
            }
            temp_row[from_node + 1] ++;
            temp_row[to_node + 1] ++;
    }
    
    /*creating the row_ptr array by finding the indexes corresponding to 
    a given node*/
    for (i = 1; i < n+1; i++){
        temp_row[i] += temp_row[i-1];
    }


    int *count = calloc(nodes, sizeof(int));
    //Starting file from line 1 again, to loop over it.
    rewind(file);
    for (i=0; i<4; i++){
        fgets(buffer, 100, file);
    }    

    size_t j;
    /*reading through the file again, and using the row_ptr array we made
    to help make a clean algorithm for col_idx. Also sorting col_idx 
    while making it, since this greatly reduces the runtime of the program*/
    for (i=0; i<edges; i++){
        fscanf(file, "%d %d", &from_node, &to_node);
            if (from_node == to_node){
                i--;
                continue;
            }
            else if (from_node < 0 || from_node >= nodes || to_node < 0 || to_node >= nodes){
                i--;
                continue;
            }

            temp_col[temp_row[from_node] + count[from_node]] = to_node;
            temp_col[temp_row[to_node] + count[to_node]] = from_node;

            /*
            sorting algorithm:
            -----------------
            constantly sorts the index 'lists' in the col_idx
            array, such that if a new element is added, it checks wether 
            its smaller than the previous element or not, and keeps swapping until 
            its bigger than the element one index behind it.
            */
            for (j=0; j<count[from_node]; j++){
                if (temp_col[temp_row[from_node] + count[from_node] - j] < temp_col[temp_row[from_node] + count[from_node] - j - 1]){
                    swap(&temp_col[temp_row[from_node] + count[from_node] - j], &temp_col[temp_row[from_node] + count[from_node] - j - 1]);
                }
                else {
                    break;
                }
            }

            for (j=0; j<count[to_node]; j++){   
                if (temp_col[temp_row[to_node] + count[to_node] - j] < temp_col[temp_row[to_node] + count[to_node] - j - 1]) {
                    swap(&temp_col[temp_row[to_node] + count[to_node] - j], &temp_col[temp_row[to_node] + count[to_node] - j - 1]);
                }
                else{
                    break;
                }
            }
            count[from_node] ++;
            count[to_node] ++;

    }

    free(count);

    *row_ptr = temp_row;
    *col_idx = temp_col;
    *N = n;

    fclose(file);
}
