#ifndef CARGACRITICA_H
#define CARGACRITICA_H
#include "Carga.h"
// Clase para cargas criticas que no se pueden apagar
class CargaCritica : public Carga {
public:
  CargaCritica(string n, float p) : Carga(n, p) {}
  bool debeencender(int hora, float precio, float nivelembalse) override {
    return true;
  }
};
#endif
