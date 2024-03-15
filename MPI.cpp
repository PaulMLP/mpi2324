#include <iostream>
#include <vector>
#include <cstring>
#include <mpi.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>

 std::vector<uint8_t> separarCanal(const uint8_t* rgb_pixels, int width, int height, int channel) {
    std::vector<uint8_t> channel_pixels(width * height);
 
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int index = (i * width + j) * 3; 
            channel_pixels[i * width + j] = rgb_pixels[index + channel];
        }
    }
 
    return channel_pixels;
}
 
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
 
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
 
    std::vector<uint8_t> rgb_image;
    int width = 0;
    int height = 0;
 
    if (rank == 0) {
        int channels;
        uint8_t* pixels = stbi_load("image01.jpg", &width, &height, &channels, STBI_rgb);
 
        rgb_image.resize(width * height * channels);
        std::memcpy(rgb_image.data(), pixels, width * height * channels);
        stbi_image_free(pixels);
    }
 
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
 
    int rows_per_rank = height / nprocs;
 
    std::vector<uint8_t> rank_rgb_pixels(rows_per_rank * width * 3);

    MPI_Scatter(rgb_image.data(), rows_per_rank * width * 3, MPI_UNSIGNED_CHAR,
                rank_rgb_pixels.data(), rows_per_rank * width * 3, MPI_UNSIGNED_CHAR,
                0, MPI_COMM_WORLD);
 
    std::vector<uint8_t> r_pixels = separarCanal(rank_rgb_pixels.data(), width, rows_per_rank, 0);
    std::vector<uint8_t> g_pixels = separarCanal(rank_rgb_pixels.data(), width, rows_per_rank, 1);
    std::vector<uint8_t> b_pixels = separarCanal(rank_rgb_pixels.data(), width, rows_per_rank, 2);
 
    std::vector<uint8_t> red_pixels;
    std::vector<uint8_t> green_pixels;
    std::vector<uint8_t> blue_pixels;

    if (rank == 0) {
        red_pixels.resize(width * height);
        green_pixels.resize(width * height);
        blue_pixels.resize(width * height);
    }
 
    MPI_Gather(r_pixels.data(), rows_per_rank * width, MPI_UNSIGNED_CHAR,
               red_pixels.data(), rows_per_rank * width, MPI_UNSIGNED_CHAR,
               0, MPI_COMM_WORLD);
 
    MPI_Gather(g_pixels.data(), rows_per_rank * width, MPI_UNSIGNED_CHAR,
               green_pixels.data(), rows_per_rank * width, MPI_UNSIGNED_CHAR,
               0, MPI_COMM_WORLD);
               
    MPI_Gather(b_pixels.data(), rows_per_rank * width, MPI_UNSIGNED_CHAR,
               blue_pixels.data(), rows_per_rank * width, MPI_UNSIGNED_CHAR,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        stbi_write_png("red.png", width, height, 1, red_pixels.data(), width);
        stbi_write_png("green.png", width, height, 1, green_pixels.data(), width);
        stbi_write_png("blue.png", width, height, 1, blue_pixels.data(), width);
    }
 
    MPI_Finalize();
    return 0;
}