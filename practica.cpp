#include <mpi.h>
#include <vector>
#include <time.h>
#include <cmath>

#define VECTOR_DIMENSION 30

int indice_min(const std::vector<int> &a, const std::vector<int> &c)
{
    std::vector<double> distancias(3);
    double min = 1000;
    int count = 0;

    for (int i = 0; i < c.size(); i += 3)
    {
        distancias[count] = std::sqrt(std::pow(a[0] - c[i], 2) + std::pow(a[1] - c[i + 1], 2) + std::pow(a[2] - c[i + 2], 2));
        count ++;
    }

    for (double d : distancias)
    {
        min = d < min ? d : min;
    }

    for (size_t i = 0; i < 3; i++)
    {
        if (min == distancias[i])
            return i;
    }

    return 0;
}

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int elements_alloc = VECTOR_DIMENSION * 3;
    int padding = 0;

    if (VECTOR_DIMENSION % nprocs != 0)
    {
        elements_alloc = std::ceil((double)VECTOR_DIMENSION / nprocs) * nprocs;
        elements_alloc *= 3;
        padding = elements_alloc - VECTOR_DIMENSION * 3;
    }

    int elements_per_rank = elements_alloc / nprocs;

    if (rank == 0)
    {
        std::printf("Dimension: %d, Elementos Asignados: %d, Padding: %d, Elementos por Rank: %d, Ranks: %d\n",
                    VECTOR_DIMENSION, elements_alloc, padding, elements_per_rank, nprocs);

        std::vector<int> A(elements_alloc);
        std::vector<int> C(9);

        srand(time(0));
        for (size_t i = 0; i < VECTOR_DIMENSION * 3; i++)
        {

            A[i] = rand() % 10 + 1;
        }

        for (size_t i = 0; i < C.size(); i++)
        {
            C[i] = rand() % 10 + 1;
        }
        /*
                for (auto e : A)
                {
                    std::printf("%d ", e);
                }

                std::printf("\nN: %ld", A.size());*/

        MPI_Scatter(A.data(), elements_per_rank, MPI_INT,
                    MPI_IN_PLACE, 0, MPI_INT,
                    0, MPI_COMM_WORLD);

        MPI_Bcast(C.data(), C.size(), MPI_INT, 0, MPI_COMM_WORLD);

        std::vector<int> aux(3);
        for (size_t i = 0; i < elements_per_rank; i += 3)
        {
            aux[0] = A[i];
            aux[1] = A[i + 1];
            aux[2] = A[i + 2];

            int indice = indice_min(aux, C);
            for (auto e : aux)
            {
                std::printf("%d, ", e);
            }
            std::printf("\t\t\t| %d\n", indice);
        }
    }
    else
    {
        std::vector<int> A_local(elements_per_rank);
        std::vector<int> C_local(9);

        MPI_Scatter(nullptr, 0, MPI_INT,
                    A_local.data(), elements_per_rank, MPI_INT,
                    0, MPI_COMM_WORLD);

        MPI_Bcast(C_local.data(), C_local.size(), MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == nprocs -1)
        {
            elements_per_rank = elements_per_rank - padding;
        }
        
        std::vector<int> aux(3);

        for (size_t i = 0; i < elements_per_rank; i += 3)
        {
            aux[0] = A_local[i];
            aux[1] = A_local[i + 1];
            aux[2] = A_local[i + 2];

            int indice = indice_min(aux, C_local);
            for (auto e : aux)
            {
                std::printf("%d, ", e);
            }
            std::printf("\t\t\t| %d\n", indice);
        }

    }

    MPI_Finalize();

    return 0;
}