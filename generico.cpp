#include <iostream>
#include <mpi.h>
#include <fstream>
#include <string>
#include <vector>

std::vector<int> read_file() {
    std::fstream fs("C:\\Users\\DELL\\Documents\\9no Semestre\\Paralela\\datos\\datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    fs.close();
    return ret;
}

int sumar(const std::vector<int>& tmp) {
    int suma = 0;
    for (int i : tmp) {
        suma += i;
    }
    return suma;
}

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    std::vector<int> data;

    if (rank == 0) {
        for (int i = 0; i < 100; i++) {
            data[i] = i;
        }
    }
    int vector_size = data.size();
    int block_size = vector_size / nprocs;

    if (rank == 0) {

        for (int rank_id = 1; rank_id < nprocs; ++rank_id) {
            std::printf("Rank_%d enviando datos a Rank_%d\n", rank, rank_id);
            int start = rank_id * block_size;
            MPI_Send(&data[start], block_size, MPI_INT, rank_id, 0, MPI_COMM_WORLD);
        }

        std::vector<int> suma_ranks(nprocs);
        suma_ranks[0] = sumar(std::vector<int>(data.begin(), data.begin() + block_size));

        for (int rank_id = 1; rank_id < nprocs; ++rank_id) {
            MPI_Recv(&suma_ranks[rank_id], 1, MPI_INT, rank_id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        std::printf("sumas parciales:");
        for (int suma : suma_ranks) {
            std::printf(" %d", suma);
        }
        std::printf("\n");

        std::printf("SUMA TOTAL: %d\n", sumar(suma_ranks));

    } else {

        std::printf("Rank_%d recibiendo datos\n", rank);

        int block_size = vector_size / nprocs;
        int receive_size = (rank == nprocs - 1) ? vector_size - rank * block_size : block_size;

        MPI_Recv(data.data(), receive_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int suma_parcial = sumar(data);
        MPI_Send(&suma_parcial, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;

}