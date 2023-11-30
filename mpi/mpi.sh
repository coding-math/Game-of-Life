#!/bin/bash

show_help() {
    echo "Uso: $0 [--hosts <numero_de_hosts>]"
    echo "Executa o comando mpirun para o Game of Life com o número de hosts especificado."
    echo "Argumentos:"
    echo "  --hosts <numero_de_hosts>: Especifica o número de hosts (padrão: 1, intervalo: 1 a 8)"
    echo "  -h, --help: Exibe esta mensagem de ajuda"
    echo
    exit 0
}

# Processa os argumentos
while [[ $# -gt 0 ]]; do
    key="$1"

    case $key in
        --hosts)
            # Verifica se foi fornecido um valor numérico
            if [[ $2 =~ ^[1-8]$ ]]; then
                hosts_number="$2"
                shift
            else
                echo "O número de hosts deve ser um inteiro entre 1 e 8."
                exit 1
            fi
            ;;
        -h|--help)
            show_help
            ;;
        *)
            echo "Opção desconhecida: $1"
            echo "Use --help para obter ajuda."
            exit 1
            ;;
    esac
    shift
done

# Se não foi especificado o número de hosts, usa o valor padrão
if [ -z "$hosts_number" ]; then
    hosts_number=1
fi

# Gera a lista de hosts
hosts_list=""
for ((i=2; i<=$hosts_number+1; i++)); do
    hosts_list+="10.0.0.$i,"
done
hosts_list=${hosts_list%,}  # Remove a vírgula final

# Executa o comando mpirun
mpirun -hosts $hosts_list -np $hosts_number ./mpi 2>/dev/null