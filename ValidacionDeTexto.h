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

#endif