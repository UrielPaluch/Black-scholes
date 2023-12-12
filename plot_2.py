import pandas as pd
import matplotlib.pyplot as plt

# Lee el archivo CSV
df = pd.read_csv('output.csv')

# Crea un objeto de figura y ejes
fig, ax = plt.subplots()

# Grafica la implied volatility en el eje Y principal
ax.plot(df['Created At'], df['Implied volatility'], label='Implied volatility', color='blue')
ax.scatter(df['Created At'], df['Implied volatility'], color='blue')  # Scatter para Implied volatility

# Grafica la realized volatility en el mismo eje Y
ax.plot(df['Created At'], df['Under volatility'], label='Under volatility', color='green')
ax.scatter(df['Created At'], df['Under volatility'], color='green')  # Scatter para Under volatility

# Configura los labels del eje x verticalmente
ax.tick_params(axis='x', rotation=90)

# Configura los labels y el título
ax.set_xlabel('Created At')
ax.set_ylabel('Volatility')
plt.title('Implied vs Realized Volatility')

# Muestra la leyenda
ax.legend()

# Muestra la imagen
plt.show()
