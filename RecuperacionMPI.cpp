#include <mpi.h>
#include <vector>
#include <time.h>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    std::printf("Rank %d of %d process \n", rank, nprocs);

int real_size = 0; 
int block_size = 0;
int padding = 0;

    if(rank==0){
        int width, height, channels;
        uint8_t* rgb_pixels = stbi_load("image01.jpg", &width, &height, &channels, STBI_rgb);

        int max_items = width*height*channels;

        real_size = max_items;
        block_size = real_size/nprocs;

        if(max_items % nprocs !=0){
            real_size = std::ceil((double)max_items/nprocs)*nprocs;
            block_size = real_size/nprocs;
            padding = real_size - max_items;
        }

        std::printf("RANK_%d: max_items: %d, real_size: %d, block_size: %d, padding: %d\n",
                    rank, max_items, real_size, block_size, padding);
    }

    MPI_Bcast( &block_size , 1 , MPI_INT , 0 , MPI_COMM_WORLD);

    if(rank==0){
            MPI_Send( &padding , 1 , MPI_INT , nprocs-1 , 0 , MPI_COMM_WORLD);
    }else if(rank==nprocs-1){
            MPI_Recv( &padding , 1 , MPI_INT , 0 , MPI_ANY_TAG , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
    }

    MPI_Finalize();
    return 0;

}