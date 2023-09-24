
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define tableSize 2048
#define A 0
#define B 1
#include <time.h>

typedef struct{
  int x;
  int y;
  int SliceSize;
}SliceRefs;

int turn = A;
float boardA[2048][2048];
float boardB[2048][2048];
int printFlag=0;
float checkAlive(int x, int y, int read){
  float count =0;
  int k;
  int flagKeepAlive = 0;
  if(read == A){
    // linha 0
    count+=boardA[((x-1+tableSize)%tableSize)][((y-1+tableSize)%tableSize)];
    count+=boardA[((x-1+tableSize)%tableSize)][((y+tableSize)%tableSize)];
    count+=boardA[((x-1+tableSize)%tableSize)][((y+1+tableSize)%tableSize)];

    // linha 1
    count+=boardA[((x+tableSize)%tableSize)][((y-1+tableSize)%tableSize)];
    count+=boardA[((x+tableSize)%tableSize)][((y+1+tableSize)%tableSize)];

    //linha 2
    count+=boardA[((x+1+tableSize)%tableSize)][((y-1+tableSize)%tableSize)];
    count+=boardA[((x+1+tableSize)%tableSize)][((y+tableSize)%tableSize)];
    count+=boardA[((x+1+tableSize)%tableSize)][((y+1+tableSize)%tableSize)];
    flagKeepAlive = boardA[x][y];

  }else{
    // linha 0
    count+=boardB[((x-1+tableSize)%tableSize)][((y-1+tableSize)%tableSize)];
    count+=boardB[((x-1+tableSize)%tableSize)][((y+tableSize)%tableSize)];
    count+=boardB[((x-1+tableSize)%tableSize)][((y+1+tableSize)%tableSize)];

    // linha 1
    count+=boardB[((x+tableSize)%tableSize)][((y-1+tableSize)%tableSize)];
    count+=boardB[((x+tableSize)%tableSize)][((y+1+tableSize)%tableSize)];

    //linha 2
    count+=boardB[((x+1+tableSize)%tableSize)][((y-1+tableSize)%tableSize)];
    count+=boardB[((x+1+tableSize)%tableSize)][((y+tableSize)%tableSize)];
    count+=boardB[((x+1+tableSize)%tableSize)][((y+1+tableSize)%tableSize)];
    flagKeepAlive = boardB[x][y];
  }

  if ((count/8.0)>0.0){
    if((count/8.0) == (2/8.0) && flagKeepAlive == 1){
        //if(printFlag) printf("vez: %d posicao ---> %d %d media: %f\n",read,x,y,count/8.0);
        return 1.0;
    }
    else if ((count/8.0) == (3/8.0)){
        //if(printFlag) printf("vez: %d posicao ---> %d %d media: %f\n",read,x,y,count/8.0);
        return 1.0;
    }
    else return 0.0;
    }

    return 0.0;
  }

void *analyzeSubMatrix(void *args){
  int i,j;
  SliceRefs *slice = (SliceRefs *)args;
  // printf("criou %d\n",slice->x);
  //printf("calculando -->%d vez-->:%d\n",slice->x, turn);

  // if (printFlag) printf("Range de calculo: (%d,%d) - (%d,%d)\n", slice->x,slice->y, slice->x + slice->SliceSize,slice->y + slice->SliceSize);
  for(i=slice->x;i< slice->x + slice->SliceSize;i++){
    for(j=0;j<2048;j++){
      if(turn==A){
        boardB[i][j] = checkAlive(i,j,A);
      }else{
        boardA[i][j] = checkAlive(i,j,B);
      }
    }
  }

    return NULL;
}
void *clean(void *args){
  int i,j;
  SliceRefs *slice = (SliceRefs *)args;
  //printf("limpando -->%d vez-->:%d\n",slice->x, turn);
if(turn==A){
       for(i=slice->x;i< slice->x + slice->SliceSize;i++)
        for(j=slice->y;j<slice->y + slice->SliceSize;j++)
            boardA[i][j] = 0;
      }else{
        for(i=slice->x;i< slice->x + slice->SliceSize;i++)
            for(j=slice->y;j<slice->y + slice->SliceSize;j++)
            boardB[i][j] = 0;
      }
}

void setGlider(){
  //GLIDER
  int lin = 1, col = 1;
  boardA[lin  ][col+1] = 1.0;
  boardA[lin+1][col+2] = 1.0;
  boardA[lin+2][col  ] = 1.0;
  boardA[lin+2][col+1] = 1.0;
  boardA[lin+2][col+2] = 1.0;


  //R-pentomino
  lin =10; col = 30;
  boardA[lin  ][col+1] = 1.0;
  boardA[lin  ][col+2] = 1.0;
  boardA[lin+1][col  ] = 1.0;
  boardA[lin+1][col+1] = 1.0;
  boardA[lin+2][col+1] = 1.0;
}
void getAliveCells(){
  int i,j;
  float count=0;
  for(i=0;i<tableSize;i++)
    for(j=0;j<tableSize;j++)
      if (turn == B )
        count+=boardA[i][j];
      else
        count+=boardB[i][j];


  printf("Quantidade de celulas vivas: %f\n", count);
}

int main(){
  int i,j;
  clock_t start = clock();
  for(i=0;i<2048;i++)
    for(j=0;j<2048;j++)
      boardA[i][j] = boardB[i][j] = 0.0;

  int numThreads=8;
  int numGens=2000;

  // printf("Numero de threads (potencia de 2):");
  // scanf("%d", &numThreads);

  // printf("\nNumero de geracoes:\n");
  // scanf("%d", &numGens);


  int unity = 2048 / numThreads;
  SliceRefs** Slices = (SliceRefs**) malloc(sizeof(SliceRefs*));

  for(i=0;i<numThreads;i++){
    Slices[i] = (SliceRefs*)malloc(sizeof(SliceRefs));
    Slices[i]->x =  i * unity;
    Slices[i]->y = i * unity;
    Slices[i]->SliceSize = unity;
   // printf("%d, %d\n", Slices[i]->x, Slices[i]->SliceSize);
  }

  pthread_t calcThreads[numThreads];
  pthread_t cleanThreads[numThreads];
  setGlider();
  int gens =0;
  getAliveCells();
  for(gens=0;gens<numGens;gens++){
    if ((gens % 400 )==  0) printf("Geracao %d\n",gens);

    for(i=0;i<numThreads;i++)
      pthread_create(&calcThreads[i], NULL, analyzeSubMatrix, (void *)Slices[i]);
    for(i=0;i<numThreads;i++)
       pthread_join(calcThreads[i],NULL);


    for(i=0;i<numThreads;i++)
      pthread_create(&cleanThreads[i], NULL, clean, (void *)Slices[i]);
    for(i=0;i<numThreads;i++)
       pthread_join(cleanThreads[i],NULL);

    if ((gens % 400 )==  0) getAliveCells();
    if (turn ==A)
      turn =B;
    else
      turn =A;
  }
    float time = ((double)(clock()- start)) / CLOCKS_PER_SEC;
    printf("Tempo de execucao: %f segundos\n", time);

  return 0;
}

