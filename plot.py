import pandas as pd
import matplotlib.pyplot as plt
import os

# Lee el archivo CSV
df = pd.read_csv('output.csv')

# Crea un objeto de figura y ejes
fig, ax1 = plt.subplots()

# Grafica la implied volatility en el eje Y principal
ax1.plot(df['Created At'], df['Implied volatility'], label='Implied volatility', color='blue')
ax1.scatter(df['Created At'], df['Implied volatility'], color='blue')  # Scatter para Implied volatility
ax1.set_xlabel('Created At')
ax1.set_ylabel('Implied volatility', color='blue')
ax1.tick_params('y', colors='blue')

# Crea un segundo eje Y para la realized volatility
ax2 = ax1.twinx()
ax2.plot(df['Created At'], df['Under volatility'], label='Under volatility', color='green')
ax2.scatter(df['Created At'], df['Under volatility'], color='green')  # Scatter para Under volatility
ax2.set_ylabel('Under volatility', color='green')
ax2.tick_params('y', colors='green')

# Rota los labels del eje x verticalmente en ambos ejes
ax1.tick_params(axis='x', rotation=90)
ax2.tick_params(axis='x', rotation=90)

# Muestra la leyenda
fig.tight_layout()
plt.title('Implied vs Realized Volatility')

# Muestra la imagen
plt.show()
