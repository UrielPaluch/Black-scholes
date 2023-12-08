#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// Definición de la "clase" para representar el DataFrame
struct OptionData {
    double price;
    double under_price;
    std::string created_at;
};

// Función de validación para la conversión de cadena a double
bool isValidDouble(const std::string& str, double& result) {
    /* isValidDouble toma dos argumentos: un string y una variable de tipo double.

    Dentro de isValidDouble, se realiza una validación especial para garantizar
    que la cadena utilice el formato adecuado (reemplazando comas por puntos) y 
    luego intenta convertirla a un número de tipo double utilizando std::stod.

    Si la conversión es exitosa, el resultado se almacena en la variable proporcionada 
    como argumento. Esto se logra porque es pasado por referencia a la función 
    (double& result), lo que significa que cualquier cambio realizado en result 
    dentro de la función se reflejará en la variable bid fuera de la función.

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

int main() {

    // Vector para almacenar filas del DataFrame
    std::vector<OptionData> dataframe;

    // Tasa libre de riesgo constante = 100%
    int rf = 1;

    // Strike price constante 1033
    int strike = 1033;

    // Nombre del archivo CSV que deseas abrir
    std::string nombreArchivo = "Exp_Octubre.csv";

    // Crear un objeto ifstream e intentar abrir el archivo
    std::ifstream archivo(nombreArchivo);

    // Verifica si la apertura fue exitosa
    if (archivo.is_open()) {
        std::string linea;

        // Vector para almacenar elementos de cada línea
        std::vector<std::string> elementos;

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
                // Construye una estructura OptionData y agrega al DataFrame
                OptionData opcion;
                double bid;
                double ask;
                double under_bid;
                double under_ask;
                std::string created_at;

                // Intenta convertir las cadenas a números solo si no están vacías y son válidas
                if (!elementos[3].empty() && isValidDouble(elementos[3], bid) &&
                    !elementos[4].empty() && isValidDouble(elementos[4], ask) &&
                    !elementos[5].empty() && isValidDouble(elementos[5], under_bid) &&
                    !elementos[6].empty() && isValidDouble(elementos[6], under_ask) &&
                    !elementos[7].empty()) {

                    // No es necesario convertir nuevamente, ya que isValidDouble ya asigna a opcion.bid, opcion.ask, etc.
                    opcion.created_at = elementos[7];
                    opcion.price = (bid + ask) / 2;
                    opcion.under_price = (under_ask + under_bid) / 2;

                    dataframe.push_back(opcion);
                }
                
            }

        }

        // Cierra el archivo después de usarlo
        archivo.close();

        // Imprime el DataFrame
        for (const auto& row : dataframe) {
            std::cout << "Price: " << row.price << "\n"
                      << "Under Price: " << row.under_price << "\n"
                      << "Created at: " << row.created_at << "\n\n";
        }


    } else {
        std::cerr << "Error al abrir el archivo." << std::endl;
    }

    return 0;
}
