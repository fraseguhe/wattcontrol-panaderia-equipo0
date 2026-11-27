#ifndef MERCADO_H
#define MERCADO_H
#include "Constantes.h"

// Clase Mercado: determina el precio de la energia segun la hora y nivel de embalses
class Mercado {
private:
  float nivelEmbalses;

public:
  Mercado(float embalses) {
    nivelEmbalses = embalses;
  }

  void setNivelEmbalses(float nuevoNivel) {
    nivelEmbalses = nuevoNivel;
  }

  float precioPorHora(int hora) {
    int precioMinimo;
    int precioMaximo;

    if (hora <= 6) {
      precioMinimo = 398;
      precioMaximo = 640;
    } else if (hora <= 14) {
      precioMinimo = 456;
      precioMaximo = 970;
    } else if (hora <= 17) {
      precioMinimo = 596;
      precioMaximo = 1040;
    } else if (hora <= 20) {
      precioMinimo = 705;
      precioMaximo = 1612;
    } else {
      precioMinimo = 565;
      precioMaximo = 970;
    }
    return precioMinimo + rand() % (precioMaximo - precioMinimo + 1);
  }

  float getNivelEmbalses() const { return nivelEmbalses; }
};
#endif
