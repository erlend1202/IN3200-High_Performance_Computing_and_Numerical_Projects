COMPILATION:
-----------
make final 
mpirun -np (number of processors) ./final (N) (M) (K)

example of running would be:
mpirun -np 4 ./final 2000 2000 10

NOTE:
make final gives the command: 
mpicc MPI_main.c MPI_single_layer_convolution.c -o final 

if another wrapper is to be used, you can simply copy this line and 
switch out the wrapper, and the rest of the compilation is then the same.