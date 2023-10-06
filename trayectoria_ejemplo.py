import numpy as np
import matplotlib.pyplot as plt

# Parámetros
a = 1.4
b = 0.3

# Condiciones iniciales
x0 = 0.5
y0 = 0.2

# Número de iteraciones
num_iterations = 10000

# Almacenar las trayectorias
x_trajectory = []
y_trajectory = []

# Iterar el mapa y almacenar los valores
x = x0
y = y0
for _ in range(num_iterations):
    x_trajectory.append(x)
    y_trajectory.append(y)

    # Aplicar el mapa de Hénon
    x_new = 1 - a * x**2 + y
    y_new = b * x

    x, y = x_new, y_new

# Graficar la trayectoria en el plano x-y
plt.figure(figsize=(8, 6))
plt.scatter(x_trajectory, y_trajectory, s=1, c="blue", marker=".")
plt.xlabel("x")
plt.ylabel("y")
plt.title("Trayectoria del Mapa de Hénon")
plt.grid(True)
plt.show()
