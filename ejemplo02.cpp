#include <iostream>
#include <mpi.h>

int main(int argc, char** argv){

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int data[100]; //vector de tamaño 100 sin inicializar

    if(rank == 0){
        std::printf("Total ranks: %d\n", nprocs);
        //inicializamos vector
        for (int i=0; i<100;i++) {
            data[i]= i;
        }

        for (int i = 1; i < nprocs; ++i) {
            std::printf("Rank_%d enviando datos a Rank_%d\n", rank, i);
            MPI_Send(data,      //datos
                     100,       //count
                     MPI_INT,   //tipo de dato
                     i,         //rank destino
                     0,         //tag
                     MPI_COMM_WORLD //grupo
            );
        }
    } else {

        MPI_Recv(data,
                 100,       //count
                 MPI_INT,   //tipo de dato
                 0,         //rank origen
                 0,         //tag
                 MPI_COMM_WORLD, //grupo
                 MPI_STATUS_IGNORE //status
                 );

        std::string str = "";
        for (int i = 0; i < 10; ++i) {
            //std::printf("%d, ", data[i]);
            str = str + std::to_string(data[i])+",";
        }
        std::printf("Rank_%d recibiendo datos ===> %s\n",rank, str.c_str());
        //std::printf("\n");
    }

    MPI_Finalize();
    return  0;
}