#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <iostream>
#include <string>
#include <cctype>
#include "Constantes.h"

using namespace std;

// Esta funcion recibe un texto y revisa si cumple con las condiciones establecidas
// revisamos si el texto no tiene caracteres, si cumple con el mínimo de letras, y si empieza
// con mayúscula
inline bool textoValido(string texto) {
  if (texto.empty()) {
    cout << " Advertencia: el texto no puede estar vacio" << endl;
    return false;
  }
  if (static_cast<int>(texto.length()) < LETRAS_MINIMAS) {
    cout << " Advertencia: el texto debe tener minimo " << LETRAS_MINIMAS << " letras" << endl;
    return false;
  }
  if (!isupper(static_cast<unsigned char>(texto.at(0)))) {
    cout << " Advertencia: la primera letra debe ser mayuscula" << endl;
    return false;
  }
  return true;
}

// Con esta función se pide el van a pedir el nombre del negocio y del responsable
// y con la ayuda de textoValido seguimos preguntando hasta que la pregunta tenga el
// formato correcto
inline string pedirTexto(string pregunta) {
  string texto;
  do {
    cout << pregunta;
    getline(cin, texto);
  } while (!textoValido(texto));
  return texto;
}

// Se extraen estas funciones del case 1 del main para limpiar la lógica del menú
inline int pedirHora() {
  int hora;
  do {
    cout << "Hora: ";
    cin >> hora;
    if (hora < 0 || hora >= HORAS_DIA) {
      cout << " Hora invalida, intenta nuevamente (Entero entre 0 y 23)" << endl;
    }
  } while (hora < 0 || hora >= HORAS_DIA);
  return hora;
}

inline float pedirPrecio() {
  float precio;
  do {
    cout << "Precio en bolsa: ";
    cin >> precio;
    if (precio <= 0) {
      cout << " Error, valor invalido, el precio debe ser mayor a 0" << endl;
    }
  } while (precio <= 0);
  return precio;
}

inline float pedirNivelEmbalse() {
  float nivel;
  do {
    cout << "Cual es el nivel de los Embalses: ";
    cin >> nivel;
    if (nivel < 0 || nivel > EMBALSE_MAXIMO) {
      cout << " Los valores de los embalses deben estar entre 0 y 100" << endl;
    }
  } while (nivel < 0 || nivel > EMBALSE_MAXIMO);
  return nivel;
}

#endif