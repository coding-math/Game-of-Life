import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Defina os estados da matriz
def estados(filename):
    with open(filename, 'r') as file:
        lines = file.read().split('\n\n')
        states = [np.array([list(map(int, line.split())) for line in block.split('\n')]) for block in lines if block.strip()]
    return states

def contar_celulas_vivas(state):
    return np.sum(state)

states = estados('estados50x50.txt')

# Ajuste o tamanho da figura em relação às dimensões da matriz
matrix_height = matrix_width = states[0].shape[0]
fig_width = matrix_width / 10  # Ajuste o fator de escala conforme necessário
fig_height = matrix_height / 10  # Ajuste o fator de escala conforme necessário
fig = plt.figure(figsize=(fig_width, fig_height))
# Crie uma figura vazia
# fig = plt.figure()

# Função para atualizar o gráfico a cada quadro da animação
def update(frame):
    plt.clf()
    num_celulas_vivas = contar_celulas_vivas(states[frame])

    plt.imshow(states[frame], cmap='viridis', aspect='auto', interpolation='nearest')
    plt.title(f'Geração: {frame + 1}/{len(states)} | Células Vivas: {num_celulas_vivas}')

# Crie a animação
ani = animation.FuncAnimation(fig, update, frames=len(states), interval=100)    

output_file = 'animacao.gif'
ani.save(output_file, writer='pillow', fps=5)

# Exiba a animação
plt.show()
