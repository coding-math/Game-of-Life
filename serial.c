#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 2048
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

float** createGrid() {
    float **grid = (float **)  malloc(N * sizeof(float *));
    for (int i = 0; i < N; i++) {
        grid[i] = (float *) malloc(N * sizeof(float));
        for (int j = 0; j < N; j++) {
            grid[i][j] = 0.0;
        }
    }
    return grid;
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

float newCell(float **grid, int i, int j) {
    float sum = 0.0;
    // top-left neighbor
    sum += grid[(i-1+N)%N][(j-1+N)%N];
    // top neighbor
    sum += grid[(i-1+N)%N][j];
    // top-right neighbor
    sum += grid[(i-1+N)%N][(j+1)%N];
    // left neighbor
    sum += grid[i][(j-1+N)%N];
    // right neighbor
    sum += grid[i][(j+1)%N];
    // bottom-left neighbor
    sum += grid[(i+1)%N][(j-1+N)%N];
    // bottom neighbor
    sum += grid[(i+1)%N][j];
    // bottom-right neighbor
    sum += grid[(i+1)%N][(j+1)%N];

    if (sum/8.0 > 0) {
        return 1.0;
    }
    return sum/8.0;
}

int getSurvivors(float **grid) {
    int survivors = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N ; j++) {
            if (grid[i][j] > 0)
                survivors++;
        }
    }
    return survivors;
}

void startGame(float **grid, float **newGrid) {
    int neighbors;
    for (int k = 0; k < ITERATIONS; k++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N ; j++) {
                neighbors = getNeighbors(grid, i, j);
                if (grid[i][j] > 0.0)
                    newGrid[i][j] = (neighbors == 2 || neighbors == 3) ? newCell(grid, i, j) : 0.0;
                else
                    newGrid[i][j] = neighbors == 3 ? newCell(grid, i, j) : 0.0 ;
            }
        }
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N ; j++) {
                grid[i][j] = newGrid[i][j];
            }
        }
    }
}

int main (void) { 
    float **grid = createGrid();
    float **newGrid = createGrid();

    glider(grid);
    rpentomino(grid);

    printf("Initial condition: %d\n", getSurvivors(grid));
    
    clock_t begin = clock();
    startGame(grid, newGrid);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    return 0;
}