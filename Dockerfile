FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install -y mpich openssh-server

RUN ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa && \
    cp ~/.ssh/id_rsa.pub ~/.ssh/authorized_keys && \
    chmod go-rwx ~/.ssh/authorized_keys && \
    echo "Host 10.0.0.*" >> ~/.ssh/config && \
    echo "    StrictHostKeyChecking no" >> ~/.ssh/config

WORKDIR /app

COPY mpi.c .

COPY mpi.sh .

RUN chmod +x mpi.sh

RUN mpicc mpi.c -o mpi

CMD service ssh start && tail -f /dev/null