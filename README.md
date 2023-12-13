# Introducción

El ejercicio se resolvió utizando c++ (`main.cpp`) y se gráfico con python `plot_1.py`
y `plot_2.py`.

El calculo de la volatilidad implícita se utilizo el método de bisección.

Se reemplazaron lo nulos utilizando el promedio entre el valor anterior y el valor
siguiente que no sean nulos.

Se anualizo la volatilidad del subyacente multiplicando por la raíz cuadrada de
la cantidad de minutos que hay en el año en los que se pueden operar.

Dentro del archivo `Resultados.md` se encuentra una descripción mas detallada.

## Gráficos

Si existe la necesidad de ver los gráficos en detalle se pueden ejecutar los
archivos `plot_1.py` y `plot_2.py` respectivamente, gracais a que matplotlib
proporciona un entorno interactivo.

![Figura completa](plots/Figure_1.png)

![Zoom](plots/Figure_2.png)
