// Criado em: 20/09/2023
// Game of Life - Serial

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N 5
#define ITERATIONS 2000

void glider(float **grid) {
    int lin = 1, col = 1;
    grid[lin  ][col+1] = 1.0;
    grid[lin+1][col+2] = 1.0;
    grid[lin+2][col  ] = 1.0;
    grid[lin+2][col+1] = 1.0;
    grid[lin+2][col+2] = 1.0;
}

void rpentomino(float **grid) {
    int lin = 10, col = 30;
    grid[lin  ][col+1] = 1.0;
    grid[lin  ][col+2] = 1.0;
    grid[lin+1][col  ] = 1.0;
    grid[lin+1][col+1] = 1.0;
    grid[lin+2][col+1] = 1.0;

}

void startGrid(float **grid, float **newGrid) {
    grid = (float **) malloc(N * sizeof(float *));
    newGrid = (float **) malloc(N * sizeof(float *));
    for (int i = 0; i < N; i++) {
        grid[i] = (float *) malloc(N * sizeof(float));
        newGrid[i] = (float *) malloc(N * sizeof(float));
    }

    glider(grid);
    rpentomino(grid);
}

void printGrid(float **grid) {
    for (int i = 0; i < N; i++) {
        printf("\n");
        for (int j = 0; j < N; j++) {
            printf("%.0f ", grid[i][j]);
        }
    }
    printf("\n");
}

int getNeighbors(float **grid, int i, int j) {
    int neighbors = 0;
    // top-left neighbor
    if (grid[(i-1+N)%N][(j-1+N)%N] > 0)
        neighbors++;
    // top neighbor
    if (grid[(i-1+N)%N][j] > 0)
        neighbors++;
    // top-right neighbor
    if (grid[(i-1+N)%N][(j+1)%N] > 0)
        neighbors++;
    // left neighbor
    if (grid[i][(j-1+N)%N] > 0)
        neighbors++;
    // right neighbor
    if (grid[i][(j+1)%N] > 0)
        neighbors++;
    // bottom-left neighbor
    if (grid[(i+1)%N][(j-1+N)%N] > 0)
        neighbors++;
    // bottom neighbor
    if (grid[(i+1)%N][j] > 0)
        neighbors++;
    // bottom-right neighbor
    if (grid[(i+1)%N][(j+1)%N] > 0)
        neighbors++;

    return neighbors;
}

int main (void) { 
    float **grid;
    float **newGrid;

    startGrid(grid, newGrid);

    return 0;
}
