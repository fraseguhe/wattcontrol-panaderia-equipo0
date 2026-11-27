#ifndef MEDIDOR_H
#define MEDIDOR_H

// Clase Medidor que acumula el consumo de energia y su costo total
class Medidor {
private:
  float consumototalkwh;
  float costototalcop;

public:
  Medidor() : consumototalkwh(0), costototalcop(0) {}
  void registrarconsumo(float kw, float precio) {
    consumototalkwh += kw;
    costototalcop += (kw * precio);
  }
  float getconsumototalkwh() const { return consumototalkwh; }
  float getcostototalcop() const { return costototalcop; }

  // Sobrecarga: promedio de costo en COP por cantidad de periodos
  float calcularPromedio(int periodos) const {
    if (periodos <= 0) {
      return 0.0f;
    }
    return costototalcop / periodos;
  }

  void reiniciar() {
    consumototalkwh = 0;
    costototalcop = 0;
  }
};
#endif
