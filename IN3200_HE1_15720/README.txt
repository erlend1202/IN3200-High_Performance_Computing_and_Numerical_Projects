Compilation:
------------
make final
export OMP_NUM_THREADS=8
./final
make clean





read_graph_from_file:
--------------------
When reading through the .txt files, we asume that the given number of 
edges is the 'legal' amount of edges, such that an edge between 
0-0 or any other illegal edges do not contribute towards this number.
This is done because the algorithm with i-- asumes that the 
file lists the correct number of legal edges.


create_SNN_graph1:
------------------
we index the inner for-loop j, which loops over the columns of the 2Dtable,
as j=i+1 and j<N. We start at i+1 since only the elements on the right side
of the diagonal, including the diagonal, need to be calculated.
We can exclude the diagonal since we dont count edges between equal 
nodes, for eksample edge between node 0 and 0, as a legal edge, so
all the values on the diagonal are 0.