# Resultados de la prueba técnica

El objetivo de este documento es dejar plasmado minimamente el marco teórico utilizado
y analizar los resultados obtenidos a partir de la serie temporal.

Se realizó el procesamiento de los datos de Exp_Octubre.csv con el archivo `main.cpp`
que arroja el resultado output.csv y luego se graficaron con `plot.py` utilizando
pandas y matplotlib.

## Tabla de contenidos

- [Resultados de la prueba técnica](#resultados-de-la-prueba-técnica)
  - [Tabla de contenidos](#tabla-de-contenidos)
  - [Modelo de Black-Scholes-Melon](#modelo-de-black-scholes-melon)
    - [Supuestos](#supuestos)
    - [Volatilidad implícita (Implied volatility IV)](#volatilidad-implícita-implied-volatility-iv)
    - [Volatilidad Historica](#volatilidad-historica)
      - [Volatilidad histórica intradiaria](#volatilidad-histórica-intradiaria)
    - [Volatilidad implicita vs volatilidad realizada](#volatilidad-implicita-vs-volatilidad-realizada)
  - [Cálculos](#cálculos)
    - [Funciones](#funciones)
      - [findImpliedVolatility](#findimpliedvolatility)
      - [obtenerDiferenciaEnAnios](#obtenerdiferenciaenanios)
      - [replaceMissingValues](#replacemissingvalues)
      - [calculateUnderVolatility](#calculateundervolatility)
  - [Análisis](#análisis)
    - [A primera vista](#a-primera-vista)
    - [Conclusiones](#conclusiones)

## Modelo de Black-Scholes-Melon

$C_0$: Precio de la opción de compra.  
$S_0$: Precio de las acciones.  
$X$: Precio de ejercicio.  
$R_f$: Tasa libre de riesgo.  
$T$: Tiempo de expiración.  
$\sigma$: Volatilidad.  

Opción de compra Europea: solo se puede ejercer al final del periodo.
$$ C_0 = S_0 * N(d_1) - X * e^{-r*t} * N(d_2) $$

$$ d_1 = \frac{(ln(\frac{s_0}{x}) + (r + \frac{\sigma^2}{2}) * T)}{(\sigma * T^{1/2})} $$

$$ d_2 = \frac{(ln(\frac{s_0}{x}) + (r - \frac{\sigma^2}{2}) * T)}{(\sigma * T^{1/2})} $$

Donde:

$N(x)$: Es la distribución normal estandar. $N(0, 1)$

### Supuestos

- No hay pagos de dividendos.
- No hay costos transaccionales.
- La tasa libre de riesgo es conocida y constante.
- Los retornos del subyacente tienen distribución normal.

### Volatilidad implícita (Implied volatility IV)

Dado de $C_0$ se puede despejar $\sigma$ utilizando métodos numéricos.

Se refiere a la volatilidad estimada del activo subyacente que haría que el
precio teórico de una opción, calculado mediante un modelo como el de Black-Scholes,
coincida con su precio de mercado observado.

En otras palabras, la volatilidad implícita es la volatilidad que, cuando se
utiliza en un modelo de opciones, produce un valor teórico que se acerca al precio
de mercado observado de la opción. Es una medida de las expectativas del mercado
sobre la futura volatilidad del activo subyacente en un periodo determinado.

### Volatilidad Historica

Mide las fluctuaciones pasadas del subyacente en un periodo de tiempo determinado.

#### Volatilidad histórica intradiaria

Garman y Klass proponen la siguiente fórmula usando OHLC.

$$\sigma^2 = 0.5 * (ln(H) - ln(L))^2 - (2*ln(2)-1) * (ln(C) - ln(O))^2$$

Este modelo se basa en que los precios siguen un movimiento browniano estandar con
cero cambio y varianza constante e infinitesimal. Chan y Lien mostraron que la
medida es insesgada.

Para cada intervalo intradiario la ecuación anterior puede ser aplicada.

Fuente: [A PRACTICAL MODEL FOR PREDICTION OF INTRADAY VOLATILITY - Young Li - Bloomberg Enterprise Quants](https://assets.bbhub.io/professional/sites/10/intraday_volatility-3.pdf)

### Volatilidad implicita vs volatilidad realizada

La volatilidad es una métrica que mide la magnitud del cambio en los precios de
una acción. En general, cuanto mayor es la volatilidad, y por ende el riesgo,
mayor la recompensa. Si la volatilidad es baja, la prima de la opción también
sera baja. Antes de hacer un trade, es buena idea saber como varía el precio de
una acción y con que velocidad.

En un trade de opciones, ambos lados hacen una apuesta sobre la volatilidad del
subyacente. Aunque hay muchas maneras de medir la volatilidad, en general se
trabaja con dos metricas: volatilidad implícita y volatilidad histórica. La
volatilidad implícita mide las expectativas de volatilidad futura (expresada en
el precio de la prima), mientras que la volatilidad histórica mide la volatilidad
pasada del subaycente.

La combinación de estas métricas tiene una influencia directa en el precio de la
opción, especificamente el componente de la prima que varía con el paso del tiempo
que fluctua con el grado de volatilidad. Los periodos donde estas medidas indican
alta volatilidad suelen beneficiar a los vendedores de opciones, mientras que la
baja volatilidad a los compradores.

En la relación considerada entre estas dos métricas, la volatilidad histórica sirve
como base, mientras que las fluctuaciones en la volatilidad implicita definen el
valor relativo de las primas. Cuando estas dos medidas representan valores similares,
se considera que las primas están valudas correctamente. Los traders de opciones
buscan desviaciones de este equilibrio para vender las opciones que esten caras
o comprar las baratas.

Por ejemplo, cuando la volatilidad implicita es significativamente mas alta que
la histórica, se considera que las primas estan caras. Una vez que la volatilidad
regrese al promedio histórico, se cierra la posición.

Por el otro lado, los compradores de opciones tienen la ventaja cuando la volatilidad
implícita es sustancialemnte menor que la histórica. En esta situación, compras
la prima barata, y cuando tiende a la media la vendes.

## Cálculos

El objetivo de esta sección es poder especificar las definiciones que se toman en
`main.cpp`.

### Funciones

#### findImpliedVolatility

Se realiza el calculo para encontrar la volatilidad implícita a través del método
de bisección. Dados los extremos a y b (a = 0.00001 ; b = 5) se parte a la mitad
la función (en un punto p) y se evalua si el precio del opción es menor o mayor
que ese precio valuado en p. Se realizan calculos iterativos hasta que el precio
calculado en p sea equivalente al precio de la opción, donde p = $\sigma$.

Como es un método iterativo, el resultado podría no ser exacto, para evitar infinitas
iteraciones se definió una tolerancia de 0.00001 y una cantidad máxima de iteraciones
de 500.

#### obtenerDiferenciaEnAnios

Dado que todos los parámetros del modelo de BSM se definen en años, se anualiza
la diferencia en día que hay hasta la fecha de expiración.

#### replaceMissingValues

En el data set hay valores nulos, la definición que se tomo para reemplazarlos es
utilizar el promedio entre el valor anterior y el valor siguiente que no sean nulos.
De esta forma se logra suavizar la serie.

Para los casos particulares del primer y último valor, se toma exclusivamente el
primer (o último) valor disponible.

#### calculateUnderVolatility

Para calcular la volatildiad del subyacente, se utiliza la fórmula presentada en
la sección [Volatilidad histórica intradiaria](#volatilidad-histórica-intradiaria)
tomando en cuenta que el open = low = bid y el close = high = ask.

Como el timeframe es de 1 minuto pero los valores de la volatilidad implícita son
hasta finish, se multiplica el resultado de la fórmula por 60 para que los minutos
se hagan horas, luego por 24 para hacerlo días y por último por 1 + la cantidad de
años hasta expiración. Logrando la misma escala para ambas series y pudiendo hacerlas
comparables.

## Análisis

Podemos analizar el gráfico proporcionado y luego podemos ahondar en los datos
para tener un entendimiento mayor.

### A primera vista

![Figura completa](plots/Figure_1.png)

A primera vista observamos unos pocos outliers, podríamos despreciarlos
considerandolos ruidos propios del mercado, dado que tenemos data con periodicidad
de 1 minuto.

Sobre el margen derecho podemos observar como la volatilidad implicita (IV) se dispara
al final de la serie. El modelo de BSM puede no andar bien cuando las opciones estan muy
cerca de la fecha de expiracion. Hay muchas explicaciones, la que mas me convenció
fue que el modelo esta basado en retornos normales del subyacente, y en periodos muy
cortos (como las opciones que estan cerca de la fecha de expiración) puede que esto
no se cumpla, incurriendo en una volatilidad implícita mucho mayor.

### Conclusiones

Si hacemos zoom dentro de cualquier timeframe en particular se puede observar como
la volatilidad del subyacente varía teniendo como linea de tendencia la IV.

![Zoom](plots/Figure_2.png)

En mi opinion, esto se debe a que la volatilidad implicita tiene en cuenta una
mayor cantidad de eventos mientras que la volatilidad realizada solo tiene en
cuenta los eventos del periodo en el que se calcula.
