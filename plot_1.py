import pandas as pd
import matplotlib.pyplot as plt

# Lee el archivo CSV
df = pd.read_csv('output.csv')

df['Diference'] = df['Implied volatility'] - df['Under volatility']

# Crea un objeto de figura y ejes
fig, ax = plt.subplots()

# Grafica la implied volatility en el eje Y principal
ax.plot(df['Created At'], df['Diference'], label='Implied volatility - Under volatility', color='blue')

# Agrega la línea punteada en el centro
ax.axhline(y=0, color='black', linestyle='--')

# Configura los labels del eje x verticalmente
ax.tick_params(axis='x', rotation=90)

# Configura los labels y el título
ax.set_xlabel('Implied volatility - Under volatility At')
ax.set_ylabel('Volatility')
plt.title('Implied volatility vs Under volatility')

# Muestra la leyenda
ax.legend()

# Muestra la imagen
plt.show()
