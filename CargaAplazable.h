#ifndef CARGAAPLAZABLE_H
#define CARGAAPLAZABLE_H
#include "Carga.h"
#include "Constantes.h"

// Clase para cargas de las que se puede prescindir en horas pico o escasez
class CargaAplazable : public Carga {
private:
  int horasMinimas;

public:
  CargaAplazable(string n, float p, int horasMin) : Carga(n, p) {
    horasMinimas = horasMin;
  }

  bool debeencender(int hora, float precio, float nivelembalse) override {
    bool eshorapico = (hora >= PICO_INICIO && hora <= PICO_FIN);
    bool energiacara = (precio > PRECIO_MAXIMO);
    bool escasezagua = (nivelembalse < EMBALSE_MINIMO);

    return !(eshorapico || energiacara || escasezagua);
  }

  int getHorasMinimas() const { return horasMinimas; }
};
#endif
