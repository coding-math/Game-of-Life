// INTEGRANTES DO GRUPO
// GABRIEL ALMEIDA RODRIGUES PEREIRA 143229
// MATHEUS DE SOUZA MEIRE 140317
// BRUNO PIRES 139892

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define V 0        // versão: game of life 0, high life 1
#define N 2048     // tamanho da matriz
#define TAMANHO 2048
#define VIVA 1
#define MORTA 0
#define G 2000     // número de gerações

void zeros(int **matrix, int nRows) {
  int i, j;
  for (i = 0; i < nRows; i++) {
    for (j = 0; j < N; j++) {
      matrix[i][j] = 0;
    }
  }
}

void join(int **grid, int *recvbuffer, int nRows) {
  int i, j;
  for (i = 0; i < nRows+2; i++) {
    for (j = 0; j < N; j++) {
      if (i == 0 || i == nRows+1) grid[i][j] = 0;
      else grid[i][j] = recvbuffer[((i - 1) * N) + j];
    }
  }
}

void setInitGrid(int *grid) {
  int lin, col;

  for (lin = 0; lin < N; lin++) {
    for (col = 0; col < N; col++) {
      grid[(lin * N) + col] = 0;
    }
  }
  // GLIDER
  lin = 1, col = 1;
  grid[(lin * N) + col + 1] = 1;
  grid[((lin + 1) * N) + col + 2] = 1;
  grid[((lin + 2) * N) + col] = 1;
  grid[((lin + 2) * N) + col + 1] = 1;
  grid[((lin + 2) * N) + col + 2] = 1;

  //R-pentomino
  lin = 10; col = 30;
  grid[(lin * N) + col + 1] = 1;
  grid[(lin * N) + col + 2] = 1;
  grid[((lin + 1) * N) + col] = 1;
  grid[((lin + 1) * N) + col + 1] = 1;
  grid[((lin + 2) * N) + col + 1] = 1;
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

void updateNeighbors(int **grid, int **newgrid, int i, int j, int neighbors) {
  if (grid[i][j]) { // celula viva
    if (neighbors == 2 || neighbors == 3) newgrid[i][j] = 1;
    else newgrid[i][j] = 0;
  }
  else { //celula morta
    if (V == 0) { // Game of life
      if (neighbors == 3) newgrid[i][j] = 1;
      else newgrid[i][j] = 0;
    } else if (V == 1) { // High life
      if (neighbors == 3 || neighbors == 6) newgrid[i][j] = 1;
      else newgrid[i][j] = 0;
    }
  }
}

int findLivingGenerations(int **grid, int **newgrid, int nRows) {
  int i, j, neighbors, livingGenerations = 0;
  for (i = 1; i < nRows+1; i++) {
    for (j = 0; j < N; j++) {
      neighbors = getNeighbors(grid, i, j);
      updateNeighbors(grid, newgrid, i, j, neighbors);
      if (newgrid[i][j]) livingGenerations++;
    }
  }
  return livingGenerations;
}

int main(int argc, char **argv) {
  int procID, nProcs, master = 0;
  int i, j, generation, livingGenerations;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &procID);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcs);

  if(procID == master) {
    printf("Game of Life - MPI\n");
    printf("Rodando com %d processos...\n\n", nProcs);
  }

  double tempoInicio = MPI_Wtime(); //Tempo inicial da execução do programa

  int nRows, localSize, vecRows[nProcs];
  int sendcounts[nProcs], begin[nProcs];

  for (i = 0; i < nProcs; i++) {
    localSize = N/nProcs;
    begin[i] = i * localSize;

    if (i == nProcs-1) vecRows[i] = N - begin[i];
    else vecRows[i] = N/nProcs;

    sendcounts[i] = vecRows[i] * N;
    if (i > 0) begin[i] = (vecRows[i-1] * N * i);
  }

  nRows = vecRows[procID];

  int *recvbuffer = (int *) malloc(nRows * N * sizeof(int));

  // Compartilha o pedaço da matriz inicial com o processo responsável //
  if (procID == master) {
    int *initialgrid = (int *) malloc(N * N * sizeof(int));
    setInitGrid(initialgrid);
    MPI_Scatterv(initialgrid, sendcounts, begin, MPI_INT, recvbuffer,
                (nRows*N), MPI_INT, master, MPI_COMM_WORLD);
    free(initialgrid);
  } else {
    MPI_Scatterv(NULL, sendcounts, begin, MPI_INT, recvbuffer,
                (nRows*N), MPI_INT, master, MPI_COMM_WORLD);
  }

  int **grid = malloc((nRows + 2) * sizeof(int *));
  int **newgrid = malloc((nRows + 2) * sizeof(int *));

  for (i = 0; i < nRows+2; i++) {
    grid[i] = malloc(N * sizeof(int));
    newgrid[i] = malloc(N * sizeof(int));
  }
  zeros(newgrid, nRows+2);

  join(grid, recvbuffer, nRows);

  int nextProc, previousProc, finalLivingGenerations;
  if (procID == 0) previousProc = nProcs - 1;
  else previousProc = procID - 1;
  nextProc = (procID + 1) % nProcs;

  MPI_Request request;
  for (generation = 0; generation < G; generation++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if (generation%2 == 0) {
      MPI_Isend(grid[1], N, MPI_INT, previousProc, 12, MPI_COMM_WORLD, &request);
      MPI_Isend(grid[nRows], N, MPI_INT, nextProc, 12, MPI_COMM_WORLD, &request);
      MPI_Recv(grid[nRows+1], N, MPI_INT, nextProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(grid[0], N, MPI_INT, previousProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      livingGenerations = findLivingGenerations(grid, newgrid, nRows);
    } else {
      MPI_Isend(newgrid[1], N, MPI_INT, previousProc, 12, MPI_COMM_WORLD, &request);
      MPI_Isend(newgrid[nRows], N, MPI_INT, nextProc, 12, MPI_COMM_WORLD, &request);
      MPI_Recv(newgrid[nRows+1], N, MPI_INT, nextProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(newgrid[0], N, MPI_INT, previousProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      livingGenerations = findLivingGenerations(newgrid, grid, nRows);
    }

    if (generation == G-1) {
      MPI_Reduce(&livingGenerations, &finalLivingGenerations,
                 1, MPI_INT, MPI_SUM, master, MPI_COMM_WORLD);
    }
  }

  double tempoFinal = MPI_Wtime(); //Tempo do término da execução do programa

  if (procID == master) {
    printf("Tempo de execução: %.04fs\n", (tempoFinal - tempoInicio));
    printf("Número de sobreviventes na geração %d: %d\n", G, finalLivingGenerations);
  }

  free(grid);
  free(newgrid);
  MPI_Finalize();
}

/*
 * Compilação: mpicc mpi.c -o mpi
 * Execução local: mpirun -np 5 prog
*/