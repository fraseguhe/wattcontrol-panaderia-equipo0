#ifndef CARGA_H
#define CARGA_H
#include "Constantes.h"
class Carga {
protected:
  string nombre;
  float potenciakw;
  bool encendida;
  int horasuso;

public:
  Carga(string n, float p) {
    nombre = n;
    potenciakw = p;
    encendida = true;
    horasuso = 0;
  }
  // Destructor virtual para liberar memoria en subclases
  virtual ~Carga() {}

  virtual bool debeencender(int hora, float precio, float nivelembalse) = 0;
  string getnombre() const { return nombre; }
  float getpotenciakw() const { return potenciakw; }
  int getHorasUso() { return horasuso;}
  bool estaencendida() const { return encendida; }
  void setencendida(bool estado) { encendida = estado; }

  void encender() {
    encendida = true;
    horasuso++;
  }
  void apagar() { encendida = false; }

  void reiniciar() {
    horasuso = 0;
  }

  float calcularconsumotohora() const {
    if (encendida) {
      return (potenciakw);
    }
    return 0;
  }
  float calcularCostoHora(float precio) const {
    return calcularconsumotohora() * precio;
  }
};
#endif
