// INTEGRANTES DO GRUPO
// GABRIEL ALMEIDA RODRIGUES PEREIRA 143229
// MATHEUS DE SOUZA MEIRE 140317
// BRUNO PIRES 139892

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>

#define TAMANHO 2048     // tamanho da matriz
#define VIVA 1
#define MORTA 0
#define MASTER 0
#define G 2000     // número de gerações

void zeros(int **matrix, int nRows) {
  int i, j;
  for (i = 0; i < nRows; i++) {
    for (j = 0; j < TAMANHO; j++) {
      matrix[i][j] = 0;
    }
  }
}

void join(int **grid, int *recvbuffer, int nRows) {
  int i, j;
  for (i = 0; i < nRows+2; i++) {
    for (j = 0; j < TAMANHO; j++) {
      if (i == 0 || i == nRows+1) grid[i][j] = 0;
      else grid[i][j] = recvbuffer[((i - 1) * TAMANHO) + j];
    }
  }
}

void setInitGrid(int *grid) {
  int lin, col;

  for (lin = 0; lin < TAMANHO; lin++) {
    for (col = 0; col < TAMANHO; col++) {
      grid[(lin * TAMANHO) + col] = 0;
    }
  }
  // GLIDER
  lin = 1, col = 1;
  grid[(lin * TAMANHO) + col + 1] = 1;
  grid[((lin + 1) * TAMANHO) + col + 2] = 1;
  grid[((lin + 2) * TAMANHO) + col] = 1;
  grid[((lin + 2) * TAMANHO) + col + 1] = 1;
  grid[((lin + 2) * TAMANHO) + col + 2] = 1;

  //R-pentomino
  lin = 10; col = 30;
  grid[(lin * TAMANHO) + col + 1] = 1;
  grid[(lin * TAMANHO) + col + 2] = 1;
  grid[((lin + 1) * TAMANHO) + col] = 1;
  grid[((lin + 1) * TAMANHO) + col + 1] = 1;
  grid[((lin + 2) * TAMANHO) + col + 1] = 1;
}

int getNeighbors(int ** grid, int i, int j){
    int neighbors = 0;

    if (grid[ ((i - 1) + TAMANHO) % TAMANHO][j  ] == VIVA) neighbors++;
    if (grid[ ((i - 1) + TAMANHO) % TAMANHO][ ((j - 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;
    if (grid[ ((i - 1) + TAMANHO) % TAMANHO][ ((j + 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;
    
    if (grid[ ((i + 1) + TAMANHO) % TAMANHO][j  ] == VIVA) neighbors++;
    if (grid[ ((i + 1) + TAMANHO) % TAMANHO][ ((j - 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;
    if (grid[ ((i + 1) + TAMANHO) % TAMANHO][ ((j + 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;

    if (grid[i  ][ ((j - 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;
    if (grid[i  ][ ((j + 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;

    return neighbors;
}

void updateNeighbors(int **grid, int **newGrid, int i, int j, int neighbors) {
  if (grid[i][j]) { // celula viva
    if (neighbors == 2 || neighbors == 3) newGrid[i][j] = 1;
    else newGrid[i][j] = 0;
  }
  else { //celula morta
    if (neighbors == 3) newGrid[i][j] = 1;
    else newGrid[i][j] = 0;
  }
}

int livingCells(int **grid, int **newGrid, int nRows) {
  int i, j, neighbors, livingGenerations = 0;
  for (i = 1; i < nRows+1; i++) {
    for (j = 0; j < TAMANHO; j++) {
      neighbors = getNeighbors(grid, i, j);
      updateNeighbors(grid, newGrid, i, j, neighbors);
      if (newGrid[i][j]) livingGenerations++;
    }
  }
  return livingGenerations;
}

int main(int argc, char **argv) {
  int procID, nProcs;
  int i, j, generation, livingGenerations;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &procID);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcs);

  if(procID == MASTER) {
    printf("Game of Life - MPI\n");
    if (nProcs > 1) printf("Rodando com %d processos...\n\n", nProcs);
    else printf("Rodando com 1 processo...\n\n");
  }

  double tempoInicio = MPI_Wtime();

  int nRows, localSize, vecRows[nProcs];
  int sendcounts[nProcs], begin[nProcs];

  for (i = 0; i < nProcs; i++) {
    localSize = TAMANHO/nProcs;
    begin[i] = i * localSize;

    if (i == nProcs-1) vecRows[i] = TAMANHO - begin[i];
    else vecRows[i] = TAMANHO/nProcs;

    sendcounts[i] = vecRows[i] * TAMANHO;
    if (i > 0) begin[i] = (vecRows[i-1] * TAMANHO * i);
  }

  nRows = vecRows[procID];

  int *recvbuffer = (int *) malloc(nRows * TAMANHO * sizeof(int));

  if (procID == MASTER) {
    int *initialGrid = (int *) malloc(TAMANHO * TAMANHO * sizeof(int));
    setInitGrid(initialGrid);
    MPI_Scatterv(initialGrid, sendcounts, begin, MPI_INT, recvbuffer,
                (nRows*TAMANHO), MPI_INT, MASTER, MPI_COMM_WORLD);
    free(initialGrid);
  } else {
    MPI_Scatterv(NULL, sendcounts, begin, MPI_INT, recvbuffer,
                (nRows*TAMANHO), MPI_INT, MASTER, MPI_COMM_WORLD);
  }

  int **grid = malloc((nRows + 2) * sizeof(int *));
  int **newGrid = malloc((nRows + 2) * sizeof(int *));

  for (i = 0; i < nRows+2; i++) {
    grid[i] = malloc(TAMANHO * sizeof(int));
    newGrid[i] = malloc(TAMANHO * sizeof(int));
  }
  zeros(newGrid, nRows+2);

  join(grid, recvbuffer, nRows);

  int nextProc, previousProc, finalLivingGenerations;
  if (procID == 0) previousProc = nProcs - 1;
  else previousProc = procID - 1;
  nextProc = (procID + 1) % nProcs;

  MPI_Request request;
  for (generation = 0; generation < G; generation++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if (generation%2 == 0) {
      MPI_Isend(grid[1], TAMANHO, MPI_INT, previousProc, 12, MPI_COMM_WORLD, &request);
      MPI_Isend(grid[nRows], TAMANHO, MPI_INT, nextProc, 12, MPI_COMM_WORLD, &request);
      MPI_Recv(grid[nRows+1], TAMANHO, MPI_INT, nextProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(grid[0], TAMANHO, MPI_INT, previousProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      livingGenerations = livingCells(grid, newGrid, nRows);
    } else {
      MPI_Isend(newGrid[1], TAMANHO, MPI_INT, previousProc, 12, MPI_COMM_WORLD, &request);
      MPI_Isend(newGrid[nRows], TAMANHO, MPI_INT, nextProc, 12, MPI_COMM_WORLD, &request);
      MPI_Recv(newGrid[nRows+1], TAMANHO, MPI_INT, nextProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(newGrid[0], TAMANHO, MPI_INT, previousProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      livingGenerations = livingCells(newGrid, grid, nRows);
    }

    if (generation == G-1) {
      MPI_Reduce(&livingGenerations, &finalLivingGenerations,
                 1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);
    }
  }

  double tempoFinal = MPI_Wtime();

  if (procID == MASTER) {
    printf("Tempo de execução: %.04fs\n", (tempoFinal - tempoInicio));
    printf("Número de sobreviventes na geração %d: %d\n", G, finalLivingGenerations);
  }

  free(grid);
  free(newGrid);
  MPI_Finalize();
}

/*
 * Compilação: mpicc mpi.c -o mpi
 * Execução local: mpirun -np 5 prog
*/