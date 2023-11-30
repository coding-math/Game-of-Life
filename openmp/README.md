# Game of Life - OpenMP

Para executar o programa, primeiro definimos o número de threads que serão utilizadas. Para isso, basta alterar o valor da constante `NUM_THREADS` no arquivo `openmp.c`. Em seguida, compile o programa com o comando:

```bash
gcc -fopenmp openmp.c -o openmp
```

Por fim, execute o programa com o comando:

```bash
./openmp
```