#ifndef NEGOCIO_H
#define NEGOCIO_H
#include "Constantes.h"
#include "Carga.h"
#include "CargaCritica.h"
#include "CargaAplazable.h"
#include "Medidor.h"
#include "Mercado.h"

// Clase Negocio: Es la clase central que relaciona el resto de clases, a traves de esta clase se definen los metodos que usara el menu para
// simular y verificar el costo de la energia.
class Negocio {
private:
  string nombreNegocio;
  Medidor medidor;
  Carga* cargas[MAX_CARGAS];
  int contadorCargas;
  float costoSinGestor;
  bool produccionIncompleta = false;
  Mercado* mercado;
  float costoDiario[DIAS_SEMANA];
  float costoPorDiaHora[DIAS_SEMANA][HORAS_DIA];
  bool tieneDatosSemanales;

public:
  Negocio(string nombre, Mercado* m) : medidor() {
    nombreNegocio = nombre;
    mercado = m;
    contadorCargas = 0;
    costoSinGestor = 0;
    produccionIncompleta = false;
    tieneDatosSemanales = false;
    for (int d = 0; d < DIAS_SEMANA; d++) {
      costoDiario[d] = 0.0f;
      for (int h = 0; h < HORAS_DIA; h++) {
        costoPorDiaHora[d][h] = 0.0f;
      }
    }
  }

  // Este metodo determina si es posible agregar una nueva carga al negocio, teniendo en cuenta que el maximo posible es de 3
  void nuevaCarga(Carga* c) {
    if (contadorCargas < MAX_CARGAS) {
      cargas[contadorCargas] = c;
      contadorCargas++;
      cout << "Nueva Carga Agregada!" << endl;
    } else {
      cout << "No hay capacidad para una nueva carga" << endl;
    }
  }

  // Sobrecarga: promedio de consumo (kWh) si porConsumo es true, o de costo (COP) si es false
  float calcularPromedio(int periodos, bool porConsumo) const {
    if (periodos <= 0) {
      return 0.0f;
    }
    if (porConsumo) {
      return medidor.getconsumototalkwh() / periodos;
    }
    return medidor.calcularPromedio(periodos);
  }

  // Retorna el indice (0..6) del dia con mayor costo en COP
  int diaMasCaro() const {
    int indiceMayor = 0;
    for (int d = 1; d < DIAS_SEMANA; d++) {
      if (costoDiario[d] > costoDiario[indiceMayor]) {
        indiceMayor = d;
      }
    }
    return indiceMayor;
  }

  // Obtiene el dia (0..6), la hora (0..23) y el costo en COP de la hora mas cara
  void horaMasCara(int& diaResultado, int& horaResultado, float& costoHoraResultado) const {
    diaResultado = 0;
    horaResultado = 0;
    costoHoraResultado = costoPorDiaHora[0][0];

    for (int d = 0; d < DIAS_SEMANA; d++) {
      for (int h = 0; h < HORAS_DIA; h++) {
        if (costoPorDiaHora[d][h] > costoHoraResultado) {
          costoHoraResultado = costoPorDiaHora[d][h];
          diaResultado = d;
          horaResultado = h;
        }
      }
    }
  }

  // El metodo evaluar decision recibe hora, precio y nivelEmbalse, con esto cada carga determina si debe encenderse o apagarse
  void evaluarDecision(int hora, float precio, float nivelEmbalse) {
    if (nivelEmbalse < EMBALSE_MINIMO) {
      cout << "--- ALERTA RIESGO DE RACIONAMIENTO --- Los embalses estan por debajo de 40%, solo funciona la carga critica!" << endl;
    }

    float potenciaEnUso = 0;
    bool estadoCargas[MAX_CARGAS];

    for (int i = 0; i < contadorCargas; i++) {
      estadoCargas[i] = cargas[i]->debeencender(hora, precio, nivelEmbalse);
      if (estadoCargas[i]) {
        potenciaEnUso += cargas[i]->getpotenciakw();
      }
    }

    if (potenciaEnUso > POTENCIA_CONTRATADA) {
      int cargaMayor = -1;
      for (int i = 0; i < contadorCargas; i++) {
        if (estadoCargas[i] && dynamic_cast<CargaAplazable*>(cargas[i]) != nullptr) {
          if (cargaMayor == -1 || cargas[i]->getpotenciakw() > cargas[cargaMayor]->getpotenciakw()) {
            cargaMayor = i;
          }
        }
      }

      if (cargaMayor != -1) {
        estadoCargas[cargaMayor] = false;
        cout << "ALTO! La potencia en uso *** " << potenciaEnUso << " kW *** es mayor a la contratada "
             << POTENCIA_CONTRATADA << "kW " << endl;
      }
    }

    for (int i = 0; i < contadorCargas; i++) {
      cout << "  " << setw(20) << left << cargas[i]->getnombre() << right
           << (estadoCargas[i] ? " ENCENDER" : " APAGAR") << endl;
    }
  }

  void simularDia() {
    costoSinGestor = 0;
    produccionIncompleta = false;
    tieneDatosSemanales = false;
    medidor.reiniciar();

    for (int i = 0; i < contadorCargas; i++) {
      cargas[i]->reiniciar();
    }

    float nivelEmbalses = mercado->getNivelEmbalses();

    if (nivelEmbalses < EMBALSE_MINIMO) {
      cout << "--- ALERTA RIESGO DE RACIONAMIENTO --- Los embalses estan por debajo de 40%, solo funciona la carga critica!" << endl;
    }

    float potenciaTotal = 0;
    for (int i = 0; i < contadorCargas; i++) {
      potenciaTotal += cargas[i]->getpotenciakw();
    }

    for (int hora = 0; hora < HORAS_DIA; hora++) {
      float precio = mercado->precio(0, hora);

      if (precio > PRECIO_ESCASEZ && nivelEmbalses < EMBALSE_MINIMO) {
        cout << "Hora " << hora << " | precio " << precio << " ALERTA RIESGO DE RACIONAMIENTO!" << endl;
        break;
      }

      costoSinGestor += potenciaTotal * precio;

      if (rand() % FALLOS_CADA == 0) {
        float potenciaCargaCritica = 0;

        for (int i = 0; i < contadorCargas; i++) {
          if (dynamic_cast<CargaAplazable*>(cargas[i]) == nullptr) {
            cargas[i]->encender();
            potenciaCargaCritica += cargas[i]->getpotenciakw();
          } else {
            cargas[i]->apagar();
          }
        }

        medidor.registrarconsumo(potenciaCargaCritica, precio);
        cout << "Fallo en Hora: " << hora << " | Falla en la lectura, solo funciona carga critica" << endl;
        continue;
      }

      bool estadoCargas[MAX_CARGAS];
      float potenciaEnUso = 0;

      for (int i = 0; i < contadorCargas; i++) {
        estadoCargas[i] = cargas[i]->debeencender(hora, precio, nivelEmbalses);
        if (estadoCargas[i]) {
          potenciaEnUso += cargas[i]->getpotenciakw();
        }
      }

      if (potenciaEnUso > POTENCIA_CONTRATADA) {
        int cargaMayor = -1;
        for (int i = 0; i < contadorCargas; i++) {
          if (estadoCargas[i] && dynamic_cast<CargaAplazable*>(cargas[i]) != nullptr) {
            if (cargaMayor == -1 || cargas[i]->getpotenciakw() > cargas[cargaMayor]->getpotenciakw()) {
              cargaMayor = i;
            }
          }
        }

        if (cargaMayor != -1) {
          estadoCargas[cargaMayor] = false;
          cout << "ALTO! La potencia en uso *** " << potenciaEnUso << " kW *** es mayor a la contratada "
               << POTENCIA_CONTRATADA << "kW " << endl;
          potenciaEnUso -= cargas[cargaMayor]->getpotenciakw();
        }
      }

      for (int i = 0; i < contadorCargas; i++) {
        if (estadoCargas[i]) {
          cargas[i]->encender();
        } else {
          cargas[i]->apagar();
        }
      }

      medidor.registrarconsumo(potenciaEnUso, precio);
      cout << "Hora: " << hora << " --- Precio: " << precio << " --- Potencia: " << potenciaEnUso << " kW" << endl;
    }

    for (int i = 0; i < contadorCargas; i++) {
      CargaAplazable* cargaAplazable = dynamic_cast<CargaAplazable*>(cargas[i]);
      if (cargaAplazable != nullptr && cargaAplazable->getHorasUso() < cargaAplazable->getHorasMinimas()) {
        produccionIncompleta = true;
      }
    }

    if (produccionIncompleta) {
      cout << "ATENCION --- PRODUCCION INCOMPLETA / HORAS MIN NO ALCANZADAS" << endl;
    }
  }

  void simularSemana() {
    costoSinGestor = 0;
    produccionIncompleta = false;
    tieneDatosSemanales = true;
    medidor.reiniciar();

    for (int i = 0; i < contadorCargas; i++) {
      cargas[i]->reiniciar();
    }

    for (int d = 0; d < DIAS_SEMANA; d++) {
      costoDiario[d] = 0.0f;
      for (int h = 0; h < HORAS_DIA; h++) {
        costoPorDiaHora[d][h] = 0.0f;
      }
    }

    float nivelEmbalses = mercado->getNivelEmbalses();
    if (nivelEmbalses < EMBALSE_MINIMO) {
      cout << "--- ALERTA RIESGO DE RACIONAMIENTO --- Los embalses estan por debajo de 40%, solo funciona la carga critica!" << endl;
    }

    float potenciaTotal = 0;
    for (int i = 0; i < contadorCargas; i++) {
      potenciaTotal += cargas[i]->getpotenciakw();
    }

    for (int d = 0; d < DIAS_SEMANA; d++) {
      for (int hora = 0; hora < HORAS_DIA; hora++) {
        float precio = mercado->precio(0, hora);

        if (precio > PRECIO_ESCASEZ && nivelEmbalses < EMBALSE_MINIMO) {
          cout << NOMBRES_DIAS[d] << " Hora " << hora << " | precio " << precio << " ALERTA RIESGO DE RACIONAMIENTO!" << endl;
          break;
        }

        costoSinGestor += potenciaTotal * precio;

        if (rand() % FALLOS_CADA == 0) {
          float potenciaCargaCritica = 0;
          for (int i = 0; i < contadorCargas; i++) {
            if (dynamic_cast<CargaAplazable*>(cargas[i]) == nullptr) {
              cargas[i]->encender();
              potenciaCargaCritica += cargas[i]->getpotenciakw();
            } else {
              cargas[i]->apagar();
            }
          }
          float costoHoraActual = potenciaCargaCritica * precio;
          medidor.registrarconsumo(potenciaCargaCritica, precio);
          costoDiario[d] += costoHoraActual;
          costoPorDiaHora[d][hora] = costoHoraActual;
          continue;
        }

        bool estadoCargas[MAX_CARGAS];
        float potenciaEnUso = 0;

        for (int i = 0; i < contadorCargas; i++) {
          estadoCargas[i] = cargas[i]->debeencender(hora, precio, nivelEmbalses);
          if (estadoCargas[i]) {
            potenciaEnUso += cargas[i]->getpotenciakw();
          }
        }

        if (potenciaEnUso > POTENCIA_CONTRATADA) {
          int cargaMayor = -1;
          for (int i = 0; i < contadorCargas; i++) {
            if (estadoCargas[i] && dynamic_cast<CargaAplazable*>(cargas[i]) != nullptr) {
              if (cargaMayor == -1 || cargas[i]->getpotenciakw() > cargas[cargaMayor]->getpotenciakw()) {
                cargaMayor = i;
              }
            }
          }
          if (cargaMayor != -1) {
            estadoCargas[cargaMayor] = false;
            potenciaEnUso -= cargas[cargaMayor]->getpotenciakw();
          }
        }

        for (int i = 0; i < contadorCargas; i++) {
          if (estadoCargas[i]) {
            cargas[i]->encender();
          } else {
            cargas[i]->apagar();
          }
        }

        float costoHoraActual = potenciaEnUso * precio;
        medidor.registrarconsumo(potenciaEnUso, precio);
        costoDiario[d] += costoHoraActual;
        costoPorDiaHora[d][hora] = costoHoraActual;
      }
    }

    for (int i = 0; i < contadorCargas; i++) {
      CargaAplazable* cargaAplazable = dynamic_cast<CargaAplazable*>(cargas[i]);
      if (cargaAplazable != nullptr && cargaAplazable->getHorasUso() < (cargaAplazable->getHorasMinimas() * DIAS_SEMANA)) {
        produccionIncompleta = true;
      }
    }

    cout << "Simulacion semanal (7 dias) finalizada." << endl;
    if (produccionIncompleta) {
      cout << "ATENCION --- PRODUCCION INCOMPLETA / HORAS MIN NO ALCANZADAS" << endl;
    }
  }

  void resumen() {
    for (int i = 0; i < contadorCargas; i++) {
      cout << setw(20) << left << cargas[i]->getnombre() << right << cargas[i]->getHorasUso() << " horas, "
           << cargas[i]->getHorasUso() * cargas[i]->getpotenciakw() << " kWh" << endl;
    }

    cout << fixed << setprecision(0);
    cout << "Costo usando el gestor: " << medidor.getcostototalcop() << "COP" << endl;
    cout << "Costo sin usar el gestor: " << costoSinGestor << " COP" << endl;

    float ahorroTotal = costoSinGestor - medidor.getcostototalcop();
    cout << "Ahorro: " << ahorroTotal << " COP";

    if (costoSinGestor > 0) {
      cout << " |" << setprecision(1) << ahorroTotal / costoSinGestor * 100 << " %|" << endl;
    }

    if (tieneDatosSemanales) {
      int idxDiaCaro = diaMasCaro();
      int diaPico = 0;
      int horaPico = 0;
      float costoPicoHora = 0.0f;
      horaMasCara(diaPico, horaPico, costoPicoHora);

      cout << setprecision(0);
      cout << "Promedio diario costo: " << calcularPromedio(DIAS_SEMANA, false) << " COP" << endl;
      cout << setprecision(2);
      cout << "Promedio diario consumo: " << calcularPromedio(DIAS_SEMANA, true) << " kWh" << endl;
      cout << setprecision(0);
      cout << "Dia mas caro: " << NOMBRES_DIAS[idxDiaCaro] << " (" << costoDiario[idxDiaCaro] << " COP)" << endl;
      cout << "Hora mas cara: " << NOMBRES_DIAS[diaPico] << " hora " << horaPico << " (" << costoPicoHora << " COP)" << endl;
    }

    if (produccionIncompleta) {
      cout << "ATENCION: LA PRODUCCION ESTA INCOMPLETA | HORAS MIN NO ALCANZADAS" << endl;
    }
  }
};
#endif
