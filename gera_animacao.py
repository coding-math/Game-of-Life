import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Defina os estados da matriz
def estados(filename):
    with open(filename, 'r') as file:
        lines = file.read().split('\n\n')
        states = [np.array([list(map(int, line.split())) for line in block.split('\n')]) for block in lines if block.strip()]
    return states

states = estados('estados.txt')
# Crie uma figura vazia
fig = plt.figure()

# Função para atualizar o gráfico a cada quadro da animação
def update(frame):
    plt.clf()  # Limpe o gráfico atual
    plt.imshow(states[frame], cmap='binary')  # Exiba o estado atual
    plt.title(f'Frame {frame + 1}/{len(states)}')  # Título com o número do quadro

# Crie a animação
ani = animation.FuncAnimation(fig, update, frames=len(states), interval=100)    
output_file = 'animacao.gif'

# Salvar a animação em um arquivo GIF
ani.save(output_file, writer='pillow', fps=5)

# Exiba a animação
plt.show()
