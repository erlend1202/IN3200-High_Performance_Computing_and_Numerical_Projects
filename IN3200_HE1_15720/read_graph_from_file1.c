#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

/*
Creates a 2Dtable_graph format of node connectivity, which consists 
of both rows and collumns representing nodes, and if the values is
0, the nodes are not connected, if value is 1, the nodes are connected.
Also finds the number of nodes.

Parameters
----------
    filename: Filename of the .txt file we want to open and read the connectivity graph from.

Notes
-----
Allocates a 2D array inside function and then sets the variable table2D as 
that array, so that it is stored outside the function call.
Also sets N as number of nodes which we find.
*/
void read_graph_from_file1(char *filename, int *N, char ***table2D){

    FILE *file;
    file = fopen(filename, "r");
    //testing if the file is empty
    if (file == NULL) {
        printf("Failure to open file %s\n", filename);
        exit(0);
    }
    size_t i = 0;
    //skipping first 3 lines
    char buffer[100];
    for (i=0; i<3; i++){
        fgets(buffer, 100, file);
    }

    int node_count, edges;
    //storing number of edges and node_count
    sscanf(buffer, "# Nodes: %d Edges: %d ", &node_count, &edges);
    fgets(buffer, 100, file);


    
    char **temp;
    temp = (char**)malloc(node_count * sizeof(char*));

    //storing values in 2Dlist as '0'
    for (i=0; i<node_count; i++){
        temp[i] = (char*)malloc(node_count * sizeof(char));
        for (size_t j=0; j<node_count; j++){
            temp[i][j] = '0';
        }
    }

    int from_node, to_node;
    /*going through elements in the txt file and setting the index 
    values to the nodes connected as '1'.
    Also testing for illegal values or if a node is connected to itself*/
    for (i=0; i < edges; i++) {
        fscanf(file, "%d %d", &from_node, &to_node);
            if (from_node == to_node){
                i--;
                continue;
            }

            else if (from_node < 0 || from_node >= node_count || to_node < 0 || to_node >= node_count){
                i--;
                continue;
            }
            
            temp[from_node][to_node] = '1';
            temp[to_node][from_node] = '1';
    }
    
    *table2D = temp;
    *N = node_count;
}
