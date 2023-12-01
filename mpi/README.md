# Game of Life - Cluster MPI

Este documento fornece um guia passo a passo para criar e utilizar um cluster MPI com Docker no Game of Life. Certifique-se de ter Docker e Docker Compose instalados em seu sistema antes de começar.

## 1. Configurando um Cluster MPI com Docker Compose

Utilize o Docker Compose para criar um cluster com 8 contêineres, cada um baseado na imagem gerada a partir do [Dockerfile](Dockerfile). O Docker Compose simplifica a orquestração desses contêineres, gerando automaticamente instâncias configuradas de acordo com as especificações definidas no [docker-compose.yaml](docker-compose.yaml). Esta configuração cria uma rede compartilhada entre os contêineres, permitindo uma comunicação eficiente, emulando assim um cluster MPI.

Para iniciar este ambiente, execute o seguinte comando:

```bash
docker-compose up -d
```

Este comando não apenas inicia os contêineres, mas também constrói as imagens com base no Dockerfile fornecido. Cada contêiner é configurado para se comunicar na rede compartilhada, replicando as condições típicas de um cluster MPI.

Obs: Os contêineres gerados pelo Docker Compose seguem o padrão de nome "mpi-container-X", onde X é o número do contêiner de 1 a 8.

## 2. Rodar o Game of Life no Cluster MPI

Para executar o MPI dentro da rede Docker, utilize o comando `./mpi.sh --hosts <numero_de_hosts>` em qualquer um dos contêineres:

```bash
docker exec -it mpi-container-1 ./mpi.sh --hosts <numero_de_hosts>
```

Ajuste `<numero_de_hosts>` conforme necessário para a quantidade desejada de hosts.

Com esses passos, o Game of Life será executado entre os containers da rede, aproveitando a capacidade do cluster para distribuir a carga e otimizar a execução.

## 3. Finalizando o Cluster MPI

Para finalizar o cluster, execute o seguinte comando:

```bash
docker-compose down
```

Este comando irá parar e remover todos os contêineres e redes criados pelo Docker Compose.
