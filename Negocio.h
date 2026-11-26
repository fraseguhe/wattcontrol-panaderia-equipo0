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

public:

  Negocio(string nombre, Mercado* m) : medidor() {
    nombreNegocio = nombre;
    mercado = m;
    contadorCargas = 0;
    costoSinGestor = 0;
    produccionIncompleta = false;
  }
  // Este metodo determina si es posible agregar una nueva carga al negocio, teniendo en cuenta que el maximo posible es de 3
  void nuevaCarga(Carga* c) {
    if(contadorCargas < MAX_CARGAS) {
      cargas[contadorCargas] = c;
      contadorCargas++;
      cout << "Nueva Carga Agregada!" << endl;
    } else {
      cout << "No hay capacidad para una nueva carga" << endl;
      }
    }

    // El metodo evaluar decision recibe hora, precio y nivelEmbalse, con esto cada carga determina si debe encenderse o apagarse
    

  void evaluarDecision(int hora, float precio, float nivelEmbalse) {

    if(nivelEmbalse < EMBALSE_MINIMO) {
      cout << "--- ALERTA RIESGO DE RACIONAMIENTO --- Los embalses estan por debajo de 40%, solo funciona la carga critica!" << endl; 
    }

    float potenciaEnUso = 0;
    bool estadoCargas[MAX_CARGAS];

    // Recorremos todas las cargas y revisamos con el metodo debeencender si dadas las condiciones ingresadas se prenden o apagan 
    for (int i = 0; i < contadorCargas; i++) {
      estadoCargas[i] = cargas[i]->debeencender(hora, precio, nivelEmbalse);

    // Vamos sumando la potencia utilizada en los casos en los que las cargas estan encendidas
    if (estadoCargas[i]) {
      potenciaEnUso += cargas[i]->getpotenciakw();
      }
    }

    // Se define la regla 6 si la potencia que esta siendo utilizada es mayor a la contratada, se apaga la carga aplazable de mayor potencia
    if(potenciaEnUso > POTENCIA_CONTRATADA) {
      int cargaMayor = -1;

    // Aqui recorremos nuevamente las cargas y usamos casting para determinar si la carga que esta siendo analizada es del tipo aplazable
    // En el caso de que lo sea se determina su potencia para luego compararla con la carga restante y decidir cual de las dos tiene mayor
    // Potencia
    for (int i = 0; i < contadorCargas; i++) {
      if(estadoCargas[i] && dynamic_cast<CargaAplazable*>(cargas[i]) != nullptr) {
        if(cargaMayor == -1 || cargas[i]->getpotenciakw() > cargas[cargaMayor]->getpotenciakw()) {
          cargaMayor = i;
        }
      }
    }

    // Cuando se determino que la potencia esta excediendo el limite, se usa el valor de cargaMayor para apagar la carga aplazable de mayor potencia
    // Tambien damos una advertencia
    if (cargaMayor != -1)
    {
      estadoCargas[cargaMayor] = false;
      cout << "ALTO! La potencia en uso *** " << potenciaEnUso << " kW *** es mayor a la contratada "
           << POTENCIA_CONTRATADA << "kW " << endl;
    }
    }

    // Finalmente se recorren las cargas y dependiendo de si estas deben encenderse o apagarse se devuelve el mensaje en pantalla
    for (int i = 0; i < contadorCargas; i++)
    {
      cout << "  " << setw(20) << left << cargas[i]->getnombre() << right
           << (estadoCargas[i] ? " ENCENDER" : " APAGAR") << endl;
    }
  }

  void simularDia() {
    // Cada vez que se usa la opcion 3 se reinician los valores para que se vuelvan a elegir
    // valores aleatorios, para esto usamos los metodos reiniciar tanto de carga como de medidor
    costoSinGestor = 0;
    produccionIncompleta = false;
    medidor.reiniciar();

    for(int i = 0; i < contadorCargas; i++) {
      cargas[i]->reiniciar();
    }

    float nivelEmbalses = mercado->getNivelEmbalses();

    if(nivelEmbalses < EMBALSE_MINIMO) {
      cout << "--- ALERTA RIESGO DE RACIONAMIENTO --- Los embalses estan por debajo de 40%, solo funciona la carga critica!" << endl; 
    }

    float potenciaTotal = 0;

    // Primero determinamos la potencia total usada por todas las cargas
    for (int i = 0; i < contadorCargas; i++)
    {
      potenciaTotal += cargas[i]->getpotenciakw();
    }

    // Con este ciclo recorremos todas las horas del dia una por una
    for (int hora = 0; hora < HORAS_DIA; hora++)
    {
      // En cada hora se determina un precio que precioPorHora define al azar en el rango establecido por mercado
      float precio = mercado->precioPorHora(hora);

      // Se detiene la simulacion si no se dan las condiciones de precio y niveles de embalse (Punto 7 de la actividad)
      if (precio > PRECIO_ESCASEZ && nivelEmbalses < EMBALSE_MINIMO)
      {
        cout << "Hora " << hora << " | precio " << precio << " ALERTA RIESGO DE RACIONAMIENTO!" << endl;
        break;
      }

      costoSinGestor += potenciaTotal * precio;

      // Al azar puede fallar la lectura, segun la actividad solo funciona la carga critica y se continua a la siguiente hora
      if (rand() % FALLOS_CADA == 0)
      {
        float potenciaCargaCritica = 0;

        // Aqui el casting lo que hace es determinar si el objeto es de tipo cargaCritica, en ese caso lo enciende.
        for (int i = 0; i < contadorCargas; i++)
        {
          if (dynamic_cast<CargaAplazable *>(cargas[i]) == nullptr)
          {
            cargas[i]->encender();
            potenciaCargaCritica += cargas[i]->getpotenciakw();
          }
          else
          {
            cargas[i]->apagar();
          }
        }

        medidor.registrarconsumo(potenciaCargaCritica, precio);
        cout << "Fallo en Hora: " << hora << " | Falla en la lectura, solo funciona carga critica" << endl;
        continue;
      }

      bool estadoCargas[MAX_CARGAS];
      float potenciaEnUso = 0;

      // Aqui inicia la logica del gestor, apoyandonos del metodo debeencender, cada carga
      // Va a determinar si debe estar encendida o apagada, y en el primer caso se va acumulando
      // La potencia que va usando
      for (int i = 0; i < contadorCargas; i++)
      {
        estadoCargas[i] = cargas[i]->debeencender(hora, precio, nivelEmbalses);
        if (estadoCargas[i])
        {
          potenciaEnUso += cargas[i]->getpotenciakw();
        }
      }

      // Aqui se aplica la regla 6 del proyecto aula, si la potencia que esta siendo utilizada en una hora
      // es mayor a la potencia contratada, la potencia aplazable de mayor valor se apaga.

      if (potenciaEnUso > POTENCIA_CONTRATADA)
      {
        int cargaMayor = -1;
        for (int i = 0; i < contadorCargas; i++)
        {
          if (estadoCargas[i] && dynamic_cast<CargaAplazable *>(cargas[i]) != nullptr)
          {
            if (cargaMayor == -1 || cargas[i]->getpotenciakw() > cargas[cargaMayor]->getpotenciakw())
            {
              cargaMayor = i;
            }
          }
        }

        if (cargaMayor != -1)
        {
          estadoCargas[cargaMayor] = false;
          cout << "ALTO! La potencia en uso *** " << potenciaEnUso << " kW *** es mayor a la contratada "
               << POTENCIA_CONTRATADA << "kW " << endl;
          potenciaEnUso -= cargas[cargaMayor]->getpotenciakw();
        }
      }

      // Aqui se encienden o apagan las cargas dependiendo de los datos arrojados
      // en los ciclos superiores y los valores que se determinaron en ese momento
      for (int i = 0; i < contadorCargas; i++)
      {
        if (estadoCargas[i])
        {
          cargas[i]->encender();
        }
        else
        {
          cargas[i]->apagar();
        }
      }

      // Se registra el consumo de la hora en el medidor
      medidor.registrarconsumo(potenciaEnUso, precio);
      cout << "Hora: " << hora << " --- Precio: " << precio << " --- Potencia: " << potenciaEnUso << " kW" << endl;
    }

    // Despues de recorrer todo el dia revisamos si se lograron las horas minimas de las cargas prescindibles
    for (int i = 0; i < contadorCargas; i++){
      CargaAplazable* cargaAplazable = dynamic_cast<CargaAplazable*>(cargas[i]);

      if(cargaAplazable != nullptr && cargaAplazable->getHorasUso() < cargaAplazable->getHorasMinimas()) {
        produccionIncompleta = true;
      }  
    }

    if (produccionIncompleta){
      cout << "ATENCION --- PRODUCCION INCOMPLETA / HORAS MIN NO ALCANZADAS" << endl;
    }
  }

  void resumen() {
    
    // Recorremos las cargas observamos las horas de uso despues de la simulacion ademas del costo 
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

    if (produccionIncompleta) {
      cout << "ATENCION: LA PRODUCCION ESTA INCOMPLETA | HORAS MIN NO ALCANZADAS" << endl;
    }
  }
};
#endif
