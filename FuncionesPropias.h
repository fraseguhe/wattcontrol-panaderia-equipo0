#ifndef FUNCIONESPROPIAS.H
#define FUNCIONESPROPIAS_H

// Primera función de sobrecarga, recibe kW y precio y devuelve el precio de una hora
inline float costo(float kW, float precio) {
  return kW * precio;
}

// Segunda función de sobrecarga, devuelve el total de costo de varias horas, esta función tiene como
// Principal objetivo ser de apoyo para determinar el promedio con la función que sigue
inline float costo(const float costos[], int n) {
  float total = 0;
  for (int i = 0; i < n; i++) {
    total += costos[i];
  }
  return total;
}

// Dado un vector de valores, se determina el promedio del costo de n cantidad de valores en el arreglo 

inline float promedio(const float valores[], int n) {
  if (n <= 0) {
    return 0;
  }
  return costo(valores, n) / n;
}

// Dependiendo del tipo de arreglo que se ingrese, va a determinar la hora más cara o el día más caro
inline int valorMayor(const float valores[], int n) {
  int mayor = 0;
  for (int i = 1; i < n; i++) {
    if (valores[i] > valores[mayor]) {
      mayor = i;
    }
  }
  return mayor;
}

#endif