#include <iostream>
#include <mpi.h>

int main(int argc, char** argv){

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int data[100]; //vector de tamaño 100 sin inicializar

    if(rank == 0){
        //inicializamos vector
        for (int i=0; i<100;i++) {
            data[i]= i;
        }

        std::printf("Rank %d enviando datos\n", rank);
        MPI_Send(data,      //datos
                 100,       //count
                 MPI_INT,   //tipo de dato
                 1,         //rank destino
                 0,         //tag
                 MPI_COMM_WORLD //grupo
                 );
    } else if (rank == 1){
        std::printf("Rank %d recibiendo datos\n", rank);
        MPI_Recv(data,
                 100,       //count
                 MPI_INT,   //tipo de dato
                 0,         //rank origen
                 0,         //tag
                 MPI_COMM_WORLD, //grupo
                 MPI_STATUS_IGNORE //status
                 );

        for (int i = 0; i < 100; ++i) {
            std::printf("%d, ", data[i]);
        }
        std::printf("\n");
    }
    //std::printf("Rank %d of %d process \n", rank, nprocs);

    MPI_Finalize();
    return  0;
}