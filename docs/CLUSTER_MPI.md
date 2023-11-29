# Cluster MPI

Este documento fornece um guia passo a passo para criar e utilizar um cluster MPI usando Docker. Certifique-se de ter Docker e Docker Compose instalados em seu sistema antes de começar.

## 1. Construir a imagem MPI

Execute o seguinte comando para construir a imagem game-of-life a partir do [Dockerfile](../Dockerfile):

```bash
docker build -t game-of-life .
```

## 2. Configurar o Cluster MPI com Docker Compose

Utilize o Docker Compose para criar um cluster com 8 contêineres em uma rede com base no [docker-compose.yaml](../docker-compose.yaml). Execute o seguinte comando:

```bash
docker-compose up -d
```

Isso iniciará os contêineres necessários para simular o cluster.

Obs: Os contêineres gerados pelo Docker Compose seguem o padrão de nome "game-of-life-mpi-X", onde X é o número do contêiner de 1 a 8.

## 3. Rodar o Game of Life no Cluster MPI

Para executar o MPI dentro da rede Docker, utilize o comando `./mpi.sh --hosts <numero_de_hosts>` em qualquer um dos contêineres:

```bash
docker exec -it game-of-life-mpi-1 ./mpi.sh --hosts <numero_de_hosts>
```

Ajuste `<numero_de_hosts>` conforme necessário para a quantidade desejada de hosts.

Com esses passos, o Game of Life será executado entre os containers da rede, aproveitando a capacidade do cluster para distribuir a carga e otimizar a execução.