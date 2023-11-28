// INTEGRANTES DO GRUPO
// GABRIEL ALMEIDA RODRIGUES PEREIRA 143229
// MATHEUS DE SOUZA MEIRE 140317
// BRUNO PIRES 139892

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <omp.h>
#include "mpi.h"

// Vamos desenvolver o jogo da vida de Conway
// Toda célula possui 8 outras vizinhas (horizontal, vertical e diagonal)

// Cada célula está em um dos estados VIVA(1) ou MORTA(0)
// Uma geracao é um conjunto de estados das células do tabuleiro
// As sociedades evoluem com a atualização do tabuleiro

// *** Regras ***
// 1. Celulas vivas com menos de 2(dois) vizinhas morrem por abandono
// 2. Cada célula viva com 2(dois) ou 3(três) deve permanecer viva para a próxima geração
// 3. Cada célula viva com 4(quatro) ou mais vizinhos morre por superpopulação
// 4. Cada célula morta com exatamente 3(três) vizinhos deve se tornar viva
 
// O que vamos fazer será considerando a versão Rainbow Game of Life
// Possuindo a diferença de que: as novas celulas que tornam-se vivas deverão ter como valor
// a média aritimética dos valores na vizinhança imediata. Portanto, as células vivas
// tem valor maior que zero

// Vamos programar um tabuleiro infinito, ou seja, a fronteira esquerda liga-se com a direita
// e a fronteira superior liga-se com a inferior

#define VIVA 1.0
#define MORTA 0.0
#define GERACOES 2000
#define TAMANHO 2048
#define MASTER 0

void join(float **grid, float *recvbuffer, int nRows) {
  int i, j;
  for (i = 0; i < nRows+2; i++) {
    for (j = 0; j < TAMANHO; j++) {
      if (i == 0 || i == nRows+1) grid[i][j] = 0;
      else grid[i][j] = recvbuffer[((i - 1) * TAMANHO) + j];
    }
  }
}

void inicializaGrid(float **grid){
    for (int i = 0; i < TAMANHO; i++){
        grid[i] = (float *)malloc(TAMANHO * sizeof(float));
        for (int j = 0; j < TAMANHO; j++){
            grid[i][j] = MORTA;
        }
    }
}

void estadoInicial(float **grid){

    // com a figura de um Glider
    int lin = 1, col   = 1;
    grid[lin  ][col+1]  = VIVA;
    grid[lin+1][col+2] = VIVA;
    grid[lin+2][col ]  = VIVA;
    grid[lin+2][col+1] = VIVA;
    grid[lin+2][col+2] = VIVA;
    
    // Figura de um R-pentomino
    lin = 10; col = 30;
    grid[lin  ][col+1] = VIVA;
    grid[lin  ][col+2] = VIVA;
    grid[lin+1][col  ] = VIVA;
    grid[lin+1][col+1] = VIVA;
    grid[lin+2][col+1] = VIVA;
}


// funcao que retorna a quantidade de vizinhos vivos em cada celula na posicao i,j
int getNeighbors(float ** grid, int i, int j){
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

int quantidadeCelulasVivas(float **grid){
    int celulasVivas = 0;
//#pragma omp parallel for shared(grid) reduction(+:celulasVivas)
    for (int i = 0; i < TAMANHO; i++){
        for (int j = 0; j < TAMANHO; j++){
            if (grid[i][j] == VIVA){
                celulasVivas++;
            }
        }
    }
    return celulasVivas;
}

void updateNeighbors(float **grid, float **new_grid, int i, int j, int neighbors) {
    float media_aritmetica = 0.0;
    if (grid[i][j] == VIVA) {  // Célula Viva
        if (neighbors < 2 || neighbors > 3){
            new_grid[i][j] = MORTA;
        }
        else{ 
            new_grid[i][j] = VIVA;
        }
    } else { // Célula Viva
        if (neighbors == 3){
            media_aritmetica = (float)neighbors/8.0;
            new_grid[i][j] = VIVA;
        }
    }
}

int findLivingGenerations(float **grid, float **newgrid, int nRows) {
  int i, j, neighbors, livingGenerations = 0;
  for (i = 1; i < nRows+1; i++) {
    for (j = 0; j < TAMANHO; j++) {
      neighbors = getNeighbors(grid, i, j);
      updateNeighbors(grid, newgrid, i, j, neighbors);
      if (newgrid[i][j]) livingGenerations++;
    }
  }
  return livingGenerations;
}

void salvarMatriz(float **grid, FILE *arquivo) {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            fprintf(arquivo, "%d ", (int)grid[i][j]);
        }
        fprintf(arquivo, "\n");
    }
    fprintf(arquivo, "\n");
}

int main(int argc, char **argv){
    int processId; /* rank dos processos */
    int noProcesses; /* Número de processos */
    int nameSize; /* Tamanho do nome */
    char computerName[MPI_MAX_PROCESSOR_NAME];
    int i;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    //printf("processId %d chegou aqui131231231211\n", processId);
    if ((int)processId == 0) {
        //printf("chegou aquimasterrr\n");
    }
    //printf("processId %d passou do if\n", processId);
    
    double starT = MPI_Wtime(); // Tempo inicial da execução do programa

    ///////////////////////////////

    int nRows, localSize, vecRows[noProcesses];
    int sendcounts[noProcesses], begin[noProcesses];

    for (i = 0; i < noProcesses; i++) {
        localSize = TAMANHO/noProcesses;
        begin[i] = i * localSize;

        if (i == noProcesses-1) vecRows[i] = TAMANHO - begin[i];
        else vecRows[i] = TAMANHO / noProcesses;

        sendcounts[i] = vecRows[i] * TAMANHO;
        if (i > 0) begin[i] = (vecRows[i-1] * TAMANHO * i);
    }
    if (processId == MASTER) {
       //printf("chegou aqui111");
    }

    nRows = vecRows[processId];

    float *recvbuffer = (float *) malloc(nRows * TAMANHO * sizeof(float));

    // Compartilha o pedaço da matriz inicial com o processo responsável //
    if (processId == MASTER) {
        float **initialGrid = (float **)malloc(TAMANHO * sizeof(float *));
        inicializaGrid(initialGrid);
        estadoInicial(initialGrid);
        MPI_Scatterv(initialGrid, sendcounts, begin, MPI_FLOAT, recvbuffer,
                    (nRows*TAMANHO), MPI_FLOAT, MASTER, MPI_COMM_WORLD);
        free(initialGrid);
    } else {
        MPI_Scatterv(NULL, sendcounts, begin, MPI_FLOAT, recvbuffer,
                    (nRows*TAMANHO), MPI_FLOAT, MASTER, MPI_COMM_WORLD);
    }

    ////////////////////////////////////////////////////

    float **grid = (float **)malloc(TAMANHO * sizeof(float *));
    float **new_grid = (float **)malloc(TAMANHO * sizeof(float *));

    inicializaGrid(grid);
    inicializaGrid(new_grid);
    estadoInicial(grid);

    join(grid, recvbuffer, nRows);

    int nextProc, previousProc, finalLivingGenerations, livingGenerations;
    if (processId == MASTER) previousProc = noProcesses - 1;
    else previousProc = processId - 1;
    nextProc = (processId + 1) % noProcesses;

    MPI_Request request;

    int sobreviventesGeracoes[GERACOES];
    //printf("chegou aquiantesalgo\n\n\n\n");
    for(int i = 0; i < GERACOES; i++){ 
        MPI_Barrier(MPI_COMM_WORLD);
        if (i % 2 == 0) {
        MPI_Isend(grid[1], TAMANHO, MPI_INT, previousProc, 12, MPI_COMM_WORLD, &request);
        MPI_Isend(grid[nRows], TAMANHO, MPI_INT, nextProc, 12, MPI_COMM_WORLD, &request);
        MPI_Recv(grid[nRows+1], TAMANHO, MPI_INT, nextProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(grid[0], TAMANHO, MPI_INT, previousProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        livingGenerations = findLivingGenerations(grid, new_grid, nRows);
        } else {
        MPI_Isend(new_grid[1], TAMANHO, MPI_INT, previousProc, 12, MPI_COMM_WORLD, &request);
        MPI_Isend(new_grid[nRows], TAMANHO, MPI_INT, nextProc, 12, MPI_COMM_WORLD, &request);
        MPI_Recv(new_grid[nRows+1], TAMANHO, MPI_INT, nextProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(new_grid[0], TAMANHO, MPI_INT, previousProc, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        livingGenerations = findLivingGenerations(new_grid, grid, nRows);
        }

        if (i == GERACOES-1) {
        MPI_Reduce(&livingGenerations, &finalLivingGenerations,
                    1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);
        }
        printf("Quantidade de celulas vivas Geração %d: %d\n", i, sobreviventesGeracoes[i]);
    }
    double stoP = MPI_Wtime();
    if (processId == 0) {
        printf("Tempo de execução trecho de computação das gerações: %.2fs\n", stoP - starT);
        printf("Quantidade de celulas vivas Geração %d: %d\n", GERACOES, quantidadeCelulasVivas(new_grid));
    }
    for (int i = 0; i < TAMANHO; i++){
        free(grid[i]);
        free(new_grid[i]);
    }
    free(grid);
    free(new_grid);
    MPI_Finalize();
    return 0;
}

/*
 * Compilação: mpicc mpi.c -o mpi
 * Execução local: mpirun -np 5 prog
*/