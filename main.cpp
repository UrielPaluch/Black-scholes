#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <string>
#include <cmath>
#include <ctime>
#include <chrono>
#include <filesystem>

// Función de distribución acumulativa normal estándar (CDF)
double cdf(double x) {
    return 0.5 * (1 + std::erf(x / std::sqrt(2)));
}

double calculate_d1(double S, double K, double T, double r, double sigma){
    return (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
}

// Definición de la función de Black-Scholes para opciones de compra
double blackScholesCall(double S, double K, double T, double r, double sigma) {

    double d1 = calculate_d1(S, K, T, r, sigma);

    double d2 = d1 - sigma * std::sqrt(T);

    return S * cdf(d1) - K * std::exp(-r * T) * cdf(d2);
}

double findImpliedVolatility(double S, double K, double T, double r, double optionPrice,
                              double a, double b, double tolerance, int maxIterations) {
    /* Se utiliza el metodo de biseccion para calcular la volatilidad implicita,
    este metodo define los dos extremos (a y b) y calcula el punto en el medio (p).
    En base a este punto medio se calcula el precio de la opcion y se evalua si
    hay que ir a la derecha o izquierda de p, achicando el intervalo.
    */
    double p, precio_teorico;
    
    for (int i = 0; i < maxIterations; ++i) {
        p = (a+b)/2;

        precio_teorico = blackScholesCall(S, K, T, r, p);
        
        if( fabs(precio_teorico-optionPrice) < tolerance) {
            return p;
        }

        if (optionPrice > precio_teorico) {
            a = p;
        } else {
            b = p;
        }
    }
    return -1.0;
}

// Definición de la "clase" para representar el DataFrame
struct OptionData {
    std::string description;
    int strike;
    std::string kind;
    double bid;
    double ask;
    double under_bid;
    double under_ask;
    std::string created_at;
    std::string expiration_date;
    double price;
    double intrinsic_value;
    double extrinsic_value;
    double under_price;
    double implied_volatility;
    double under_volatility;
    double expiration;
};

// Función de validación para la conversión de cadena a double
bool isValidDouble(const std::string& str, double& result) {
    /* isValidDouble toma dos argumentos: un string y una variable de tipo double.

    Dentro de isValidDouble, se realiza una validación especial para garantizar
    que la cadena utilice el formato adecuado (reemplazando comas por puntos) y 
    luego intenta convertirla a un número de tipo double utilizando std::stod.

    Si la conversión es exitosa, el resultado se almacena en la variable proporcionada 
    como argumento. Esto se logra porque es pasado por referencia a la función y
    no se le asigna `const` como al otro parametro (double& result), lo que 
    significa que cualquier cambio realizado en result  dentro de la función se 
    reflejará en la variable bid fuera de la función.

    La función isValidDouble devuelve true si la conversión es exitosa y false 
    si hay algún error durante la conversión.
    */
    std::string strWithDot = str;
    
    // Reemplazar comas por puntos en la cadena
    size_t pos = 0;

    // std::string::npos es una constante especial de la clase std::string en C++. 
    // Representa un valor que indica una posición no encontrada o inválida dentro 
    // de la cadena.
    // pos busca la posición en la que se encuentra la , en la cadena de texto.
    while ((pos = strWithDot.find(',', pos)) != std::string::npos) {
        strWithDot.replace(pos, 1, ".");
    }

    try {
        size_t pos;
        result = std::stod(strWithDot, &pos); // Se intenta convertir a double
        // Verifica si se consumieron todos los caracteres de la cadena
        return pos == strWithDot.length();
    // Si no se consumieron, hubo algun error.
    } catch (const std::invalid_argument&) {
        return false; // Error de conversión
    } catch (const std::out_of_range&) {
        return false; // Valor fuera de rango
    }
}

bool isValidFormatDate(const std::string& date) {
    // Expresión regular para el formato de fecha
    std::regex date_regex("^(0?[1-9]|1[0-2])/(0?[1-9]|1[0-9]|2[0-9]|3[0-1])/(20[0-9][0-9]) (0?[0-9]|1[0-9]|2[0-3]):([0-5][0-9])$");
    /*
    (0?[1-9]|1[0-2]): Representa el mes, permitiendo un dígito opcional para los 
    meses del 1 al 9.
    (0?[1-9]|1[0-9]|2[0-9]|3[0-1]): Representa el día, permitiendo un dígito opcional 
    para los días del 1 al 9 y dos dígitos para los días del 10 al 31.
    (20[0-9][0-9]): Representa el año, asegurándose de que sea un año válido del 
    siglo XXI.
    (0?[0-9]|1[0-9]|2[0-3]): Representa la hora en formato de 24 horas,
     permitiendo un dígito opcional para las horas del 0 al 9 y dos dígitos para 
     las horas del 10 al 23.
    ([0-5][0-9]): Representa los minutos, asegurándose de que estén en el rango 
    de 00 a 59.
    */

    // Verificar si la cadena cumple con el formato
    if (std::regex_match(date, date_regex)) {
        return true;
    } else {
        std::cout << "Formato de fecha invalida: " << date  << "\n";
        return false;
    }
}

bool isValidFormatExpirationDate(const std::string& date) {
    // Expresión regular para el formato de fecha
    std::regex date_regex("\\d{2}/\\d{2}/\\d{4}");

    // Verificar si la cadena cumple con el formato
    if (std::regex_match(date, date_regex)) {
        return true;
    } else {
        std::cout << "Formato de fecha de vencimiento invalida" << "\n";
        return false;
    }
}

// Función para obtener la diferencia en días entre dos fechas
double obtenerDiferenciaEnAnios(const std::string& fecha1_str, const std::string& fecha2_str) {
    // Convertir cadenas a tipos de fecha y hora
    std::tm tm1 = {};
    std::tm tm2 = {};

    // The std::istringstream is a string class object which is used to stream 
    // the string into different variables and similarly files can be stream into 
    // strings
    std::istringstream ss1(fecha1_str);
    std::istringstream ss2(fecha2_str);

    if (!isValidFormatDate(fecha1_str)) {
        return -1;
    }

    if (!isValidFormatExpirationDate(fecha2_str)) {
        return -1;
    }

    // Formato de fecha y hora para la primer cadena
    ss1 >> std::get_time(&tm1, "%m/%d/%Y");
    // Ignorar el espacio entre la fecha y la hora
    ss1.ignore(1);
    ss1 >> std::get_time(&tm1, "%H:%M:%S");

    // Formato de fecha para la segunda cadena
    ss2 >> std::get_time(&tm2, "%d/%m/%Y %H:%M:%S");

    // Convertir a tipos de duración
    std::time_t time1 = std::mktime(&tm1);
    std::time_t time2 = std::mktime(&tm2);

    if (time2 < time1) {
        std::cout << "Error en la fecha de expiracion"
                  << "no puede ser menor a la fecha de valuacion de la opcion";

        return -1.0;
    }

    // Primero calcula la diferencia en segundos y despues la pasa a años
    auto duracion = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::seconds(time2 - time1));
    double diferencia_en_anios = duracion.count() / (365 * 24 * 60 * 60); // Dividir por segundos en un año

    return diferencia_en_anios;
}

void saveFile(const std::vector<OptionData>& dataframe) {

    // Nombre del archivo
    std::filesystem::path archivoPath = "output.csv";

    // Abrir un archivo para escritura
    std::ofstream archivoSalida(archivoPath);

    // Encabezados
    archivoSalida << "Description,Strike,Kind,Bid,Ask,Under Bid,Under Ask,Created At,Price,Valor intrinsico,Valor extrinsico,Under Price,Implied volatility,Under volatility,Years to expiration\n";

    // Verificar si el archivo se abrió correctamente
    if (!archivoSalida.is_open()) {
        std::cerr << "No se pudo abrir el archivo de salida." << std::endl;
        return; // Salir sin escribir si hay un error
    }

    // Escribir en el archivo en lugar de imprimir en la consola
    for (const auto& row : dataframe) {
        archivoSalida << row.description << ","
                      << row.strike << ","
                      << row.kind << ","
                      << row.bid << ","
                      << row.ask << ","
                      << row.under_bid << ","
                      << row.under_ask << ","
                      << row.created_at << ","
                      << row.price << ","
                      << row.intrinsic_value << ","
                      << row.extrinsic_value << ","
                      << row.under_price << ","
                      << row.implied_volatility << ","
                      << row.under_volatility << ","
                      << row.expiration << "\n";
    }

    // Cerrar el archivo después de escribir
    archivoSalida.close();

    std::cout << "Datos guardados correctamente" << std::endl;
}

struct Data {
    std::string description;
    std::string strike;
    std::string kind;
    std::string bid;
    std::string ask;
    std::string underBid;
    std::string underAsk;
    std::string created_at;
};

void replaceMissingValues(std::vector<Data>& data){
    double bid, ask, underBid, underAsk;

    // Primera iteracion
    if(!isValidDouble(data[0].ask, ask)) {
        for (size_t i = 1; i < data.size(); i++) {
            if(isValidDouble(data[i].ask, ask)) {
                data[0].ask = data[i].ask;
                break;
            }
        } 
    }

    if(!isValidDouble(data[0].bid, bid)) {
        for (size_t i = 1; i < data.size(); i++) {
            if(isValidDouble(data[i].bid, bid)) {
                data[0].bid = data[i].bid;
                break;
            }
        } 
    }

    if(!isValidDouble(data[0].underBid, underBid)) {
        for (size_t i = 1; i < data.size(); i++) {
            if(isValidDouble(data[i].underBid, underBid)) {
                data[0].underBid = data[i].underBid;
                break;
            }
        } 
    }

    if(!isValidDouble(data[0].underAsk, underAsk)) {
        for (size_t i = 1; i < data.size(); i++) {
            if(isValidDouble(data[i].underAsk, underAsk)) {
                data[0].underAsk = data[i].underAsk;
                break;
            }
        } 
    }

    // De la segunda iteracion a la anteultima

    for (size_t i = 1; i < data.size() - 1; i++) {
        if(!isValidDouble(data[i].ask, ask)) {
            double punta_inferior = -1;
            double punta_superior = -1;

            for (size_t j = i; j >= 0; j--) {
                if(isValidDouble(data[j].ask, ask)) {
                    punta_inferior = ask;
                    break;
                }
            }

            for (size_t z = i; z < data.size(); z++) {
                if(isValidDouble(data[z].ask, ask)) {
                    punta_superior = ask;
                    break;
                }
            }

            if (punta_inferior != -1 && punta_superior != -1) {
                data[i].ask = std::to_string((punta_inferior + punta_superior) / 2);
            }


        }
    }

    for (size_t i = 1; i < data.size() - 1; i++) {
        if(!isValidDouble(data[i].bid, bid)) {
            double punta_inferior = -1;
            double punta_superior = -1;

            for (size_t j = i; j >= 0; j--) {
                if(isValidDouble(data[j].bid, bid)) {
                    punta_inferior = bid;
                    break;
                }
            }

            for (size_t z = i; z < data.size(); z++) {
                if(isValidDouble(data[z].bid, bid)) {
                    punta_superior = bid;
                    break;
                }
            }

            if (punta_inferior != -1 && punta_superior != -1) {
                data[i].bid = std::to_string((punta_inferior + punta_superior) / 2);
            }


        }
    }

    for (size_t i = 1; i < data.size() - 1; i++) {
        if(!isValidDouble(data[i].underBid, underBid)) {
            double punta_inferior = -1;
            double punta_superior = -1;

            for (size_t j = i; j >= 0; j--) {
                if(isValidDouble(data[j].underBid, underBid)) {
                    punta_inferior = underBid;
                    break;
                }
            }

            for (size_t z = i; z < data.size(); z++) {
                if(isValidDouble(data[z].underBid, underBid)) {
                    punta_superior = underBid;
                    break;
                }
            }

            if (punta_inferior != -1 && punta_superior != -1) {
                data[i].underBid = std::to_string((punta_inferior + punta_superior) / 2);
            }


        }
    }

    for (size_t i = 1; i < data.size() - 1; i++) {
        if(!isValidDouble(data[i].underAsk, underAsk)) {
            double punta_inferior = -1;
            double punta_superior = -1;

            for (size_t j = i; j >= 0; j--) {
                if(isValidDouble(data[j].underAsk, underAsk)) {
                    punta_inferior = underAsk;
                    break;
                }
            }

            for (size_t z = i; z < data.size(); z++) {
                if(isValidDouble(data[z].underAsk, underAsk)) {
                    punta_superior = underAsk;
                    break;
                }
            }

            if (punta_inferior != -1 && punta_superior != -1) {
                data[i].underAsk = std::to_string((punta_inferior + punta_superior) / 2);
            }


        }
    } 

    // ultima iteracion

    if(!isValidDouble(data[data.size()].ask, ask)) {
        for (size_t i = data.size(); i >= 0; i--) {
            if(isValidDouble(data[i].ask, ask)) {
                data[data.size()].ask = data[i].ask;
                break;
            }
        } 
    }

    if(!isValidDouble(data[data.size()].bid, bid)) {
        for (size_t i = data.size(); i >= 0; i--) {
            if(isValidDouble(data[i].bid, bid)) {
                data[data.size()].bid = data[i].bid;
                break;
            }
        } 
    }

    if(!isValidDouble(data[data.size()].underAsk, underAsk)) {
        for (size_t i = data.size(); i >= 0; i--) {
            if(isValidDouble(data[i].underAsk, underAsk)) {
                data[data.size()].underAsk = data[i].underAsk;
                break;
            }
        } 
    }

    if(!isValidDouble(data[data.size()].underBid, underBid)) {
        for (size_t i = data.size(); i >= 0; i--) {
            if(isValidDouble(data[i].underBid, underBid)) {
                data[data.size()].underBid = data[i].underBid;
                break;
            }
        } 
    }


    return;
}

double calculateUnderVolatility(const double& bid, const double& ask, const double& expiration) {
    double logDifference = std::log(bid) - std::log(ask);
    double term1 = 0.5 * std::pow(logDifference, 2);
    double term2 = (2 * std::log(2) - 1) * std::pow(logDifference, 2);

    // 6 horas y media de ruedas diaria. 6.5 x 60 = 390
    // 256 son los dias que se pueden operar (aproximadamente) en un año
    return std::sqrt(term1 - term2) * std::sqrt(256 * 390) ;
}

int main() {

    // Vector para almacenar filas del DataFrame
    std::vector<OptionData> dataframe;

    // Tasa libre de riesgo constante = 100%
    // TNA
    int rf = 1;
    double rf_continua = std::log(1 + rf);

    // Strike price constante 1033
    int strike = 1033;

    // Las opciones expiran el tercer viernes de cada mes.
    // En el caso de GFGC1033OC, expira el 20/10.
    // El formato siempre es dd/mm/YYYY
    std::string fecha_vencimiento = "20/10/2023";

    if (!isValidFormatExpirationDate(fecha_vencimiento)) {
        return 0;
    }

    // Para hacer la interpolacion
    double tolerance = 0.00001; // Tolerancia
    int max_iterations = 500;  // Número máximo de iteraciones

    // Nombre del archivo CSV que deseas abrir
    std::string nombreArchivo = "Exp_Octubre.csv";

    // Crear un objeto ifstream e intentar abrir el archivo
    std::ifstream archivo(nombreArchivo);

    std::vector<Data> datos;

    // Verifica si la apertura fue exitosa
    if (archivo.is_open()) {
        std::string linea;

        // Vector para almacenar elementos de cada línea
        std::vector<std::string> elementos;

        // Leer la primera línea (encabezados)
        std::getline(archivo, linea);

        // Lee cada línea del archivo
        while (std::getline(archivo, linea)) {
            // Esta clase te permite tratar una cadena de caracteres como si 
            // fuera una secuencia, Ejemplo:
            // GFGC1033OC;1033;CALL;130;178,999;1180,5;1184,85;10/18/2023 12:18
            // Me permite trabajar con cada elemento por separado 
            std::istringstream streamLinea(linea);
            std::string valor;

            // Vector para almacenar elementos de la línea actual
            std::vector<std::string> elementos;

            // Lee y almacena cada elemento separado por ;
            while (std::getline(streamLinea, valor, ';')) {
                elementos.push_back(valor);
            }

            // Verifica si hay suficientes elementos para construir una fila
            if (elementos.size() >= 8) {
                Data dato;

                dato.description = elementos[0];
                dato.strike = elementos[1];
                dato.kind = elementos[2];
                dato.bid = elementos[3];
                dato.ask = elementos[4];
                dato.underBid = elementos[5];
                dato.underAsk = elementos[6];
                dato.created_at = elementos[7];

                datos.push_back(dato);
            }
        }
    } else {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return 0;
    }

    // Cierra el archivo después de usarlo
    archivo.close();

    replaceMissingValues(datos);

        
    // Verifica si hay suficientes elementos para construir una fila
    for (size_t i = 0; i < datos.size(); i++) {
        // Construye una estructura OptionData y agrega al DataFrame
        OptionData opcion;
        double bid = -1.0;
        double ask = -1.0;
        double under_bid = -1.0;
        double under_ask = -1.0;

        // Valida si los elementos no estan vacios y son del tipo double
        // La funcion isValidDouble devuelve true o false, pero modifica
        // el segundo parametro que se le pasa entonces lo puedo usar
        // ya transformado al tipo double.

        // Valido con una expresion regular que la fecha tenga siempre
        // el mismo formato.
        if (!datos[i].created_at.empty()) {
            opcion.expiration = obtenerDiferenciaEnAnios(datos[i].created_at, 
                                                         fecha_vencimiento);
        }

        if (isValidDouble(datos[i].bid, bid) &&
            isValidDouble(datos[i].ask, ask)) {
                opcion.price = (bid + ask) / 2;
        }

        if (isValidDouble(datos[i].underBid, under_bid) &&
            isValidDouble(datos[i].underAsk, under_ask)) {
                opcion.under_price = (under_ask + under_bid) / 2;
                opcion.under_volatility = calculateUnderVolatility(under_bid, under_ask, opcion.expiration);
        }

        opcion.implied_volatility = -1;

        // Si todas las validaciones fueron correctas calcula la
        // volatilidad implicita
        if (opcion.expiration > 0 && 
            opcion.price > 0 && 
            opcion.under_price > 0) {

            opcion.implied_volatility = findImpliedVolatility(opcion.under_price, 
            strike, opcion.expiration, rf_continua, opcion.price, 0.00001, 5, 
            tolerance, max_iterations);
        }

        opcion.description = "GFGC1033OC";
        opcion.strike = 1033;
        opcion.kind = "CALL";
        opcion.bid = bid;
        opcion.ask = ask;
        opcion.under_ask = under_ask;
        opcion.under_bid = under_bid;
        opcion.created_at = datos[i].created_at;
        opcion.expiration_date = fecha_vencimiento;
        opcion.intrinsic_value = opcion.under_price - opcion.strike;
        opcion.extrinsic_value = opcion.price - opcion.intrinsic_value;

        dataframe.push_back(opcion);
    }

    saveFile(dataframe);

    return 0;
}
