// Importe de librerias
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

// Directivas
#define HORAS_DIA 24
#define MAX_CARGAS 3

// CONSTANTES DE LA PANADERIA
const float POTENCIA_CONTRATADA = 15.0; // KW maximos para la panaderia
const float PRECIO_MAXIMO = 800;  
const float PRECIO_ESCASEZ = 1500.0;      // PRECIO MAXIMO POR KW
const float EMBALSE_MINIMO = 40;       // CAPACIDAD MINIMA DE EMBALSE

// POTENCIA DE CADA CARGA EN EL NEGOCIO
const float POTENCIA_NEVERA = 1.2;
const float POTENCIA_HORNO = 7.0;
const float POTENCIA_AMASADORA = 2.2;

// HORAS PICO
const int PICO_INICIO = 18;
const int PICO_FIN = 20;

// OTRAS CONSTANTES
const int FALLOS_CADA = 20;

// Clases
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

  virtual bool debeencender(int hora, float preccio, float nivelembalse) = 0;
  string getnombre() const { return nombre; }
  float getpotenciakw() const { return potenciakw; }
  bool estaencendida() const { return encendida; }
  void setencendida(bool estado) { encendida = estado; }

  void encender() {
    encendida = true;
    horasuso++;
  }
  void apagar() { encendida = false; }
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

// Clase para cargas criticas que no se pueden apagar
class CargaCritica : public Carga {
public:
  CargaCritica(string n, float p) : Carga(n, p) {}
  bool debeencender(int hora, float precio, float nivelembalse) override {
    return true;
  }
};

// Clase para cargas de las que se puede prescindir en horas pico o escasez
class CargaPresindible : public Carga {
public:
  CargaPresindible(string n, float p) : Carga(n, p) {}
  bool debeencender(int hora, float precio, float nivelembalse) override {
    bool eshorapico = (hora >= PICO_INICIO && hora <= PICO_FIN);
    bool energiacara = (precio > PRECIO_MAXIMO);
    bool escasezagua = (nivelembalse < EMBALSE_MINIMO);

    if ((eshorapico && energiacara) || escasezagua) {
      return false;
    }
    return true;
  }
};

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
  void reiniciar() {
    consumototalkwh = 0;
    costototalcop = 0;
  }
};

// Clase Mercado: determina el precio de la energia segun la hora y nivel de embalses
class Mercado {
private:
  float nivelEmbalses;

public:
  Mercado(float embalses) {
    nivelEmbalses = embalses;
  }

  public:

    Mercado(float embalses) {
        nivelEmbalses = embalses;
    }


    float precioPorHora(int hora) {
        int precioMinimo;
        int precioMaximo;

        if (hora <= 6) {
            precioMinimo = 398; precioMaximo = 640;
        } else if (hora <= 14) {
            precioMinimo = 456; precioMaximo = 970;
        } else if (hora <= 17) {
            precioMinimo = 596; precioMaximo = 1040;
        } else if (hora <= 20) {
            precioMinimo = 705; precioMaximo = 1612;
        } else {
            precioMinimo = 565; precioMaximo = 970;
        }
        return precioMinimo + rand() % (precioMaximo - precioMinimo + 1);
    }

    float getNivelEmbalses() {return nivelEmbalses;}
};


// Clase Negocio: Es la clase central que relaciona el resto de clases, a traves de esta clase se definen los metodos que usara el menu para
// simular y verificar el costo de la energia.
class Negocio {
private:

    string nombreNegocio;
    Medidor medidor;
    Carga* cargas[MAX_CARGAS];
    int contadorCargas;
    float costoSinGestor;
    Mercado* mercado;

public:

    Negocio(string nombre, Mercado* m) : medidor() {
        nombreNegocio = nombre;
        mercado = m;
        contadorCargas = 0;
        costoSinGestor = 0;
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
                if(estadoCargas[i] && dynamic_cast<CargaPresindible*>(cargas[i]) != nullptr) {
                    if(cargaMayor == -1 || cargas[i]->getpotenciakw() > cargas[cargaMayor]->getpotenciakw()) {
                        cargaMayor = i;
                    }
                }
            }

            // Cuando se determino que la potencia esta excediendo el limite, se usa el valor de cargaMayor para apagar la carga aplazable de mayor potencia
            // Tambien damos una advertencia
            if(cargaMayor != -1) {
                estadoCargas[cargaMayor] = false;
                cout << "ALTO! La potencia en uso *** " << potenciaEnUso << " kW *** es mayor a la contratada "
                << POTENCIA_CONTRATADA << "kW " << endl;
            }
        }

        // Finalmente se recorren las cargas y dependiendo de si estas deben encenderse o apagarse se devuelve el mensaje en pantalla
        for (int i = 0; i < contadorCargas; i++) {
             cout << "  " << setw(20) << left << cargas[i]->getnombre() << right
             << (estadoCargas[i] ? " ENCENDER" : " APAGAR") << endl;
        }
    }

     void simularDia() {
        costoSinGestor = 0;
        float nivelEmbalses = mercado->getNivelEmbalses();
        float potenciaTotal = 0;
        
        // Primero determinamos la potencia total usada por todas las cargas
        for(int i = 0; i < contadorCargas; i++) {
            potenciaTotal += cargas[i]->getpotenciakw();
        }

        // Con este ciclo recorremos todas las horas del dia una por una
        for(int hora = 0; hora < HORAS_DIA; hora++) {
            // En cada hora se determina un precio que precioPorHora define al azar en el rango establecido por mercado
            float precio = mercado->precioPorHora(hora);

            // Se detiene la simulacion si no se dan las condiciones de precio y niveles de embalse (Punto 7 de la actividad)
            if(precio > PRECIO_ESCASEZ && nivelEmbalses < EMBALSE_MINIMO) {
                cout << "Hora " << hora << " | precio " << precio << " ALERTA RIESGO DE RACIONAMIENTO!" << endl;
                break;
            }


            costoSinGestor += potenciaTotal * precio;

            // Al azar puede fallar la lectura, segun la actividad solo funciona la carga critica y se continua a la siguiente hora
            if(rand() % FALLOS_CADA == 0) {
                float potenciaCargaCritica = 0;

                // Aqui el casting lo que hace es determinar si el objeto es de tipo cargaCritica, en ese caso lo enciende.
                for(int i = 0; i < contadorCargas; i++) {
                    if(dynamic_cast<CargaPresindible*>(cargas[i]) == nullptr) {
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


            // Aqui inicia la logica del gestor, apoyandonos del metodo debeencender, cada carga
            // Va a determinar si debe estar encendida o apagada, y en el primer caso se va acumulando
            // La potencia que va usando
            for(int i = 0; i < contadorCargas; i++) {
                estadoCargas[i] = cargas[i]->debeencender(hora, precio, nivelEmbalses);
                if(estadoCargas[i]) {
                    potenciaEnUso += cargas[i]->getpotenciakw();
                }
            }

            // Aqui se aplica la regla 6 del proyecto aula, si la potencia que esta siendo utilizada en una hora
            // es mayor a la potencia contratada, la potencia aplazable de mayor valor se apaga.

            if (potenciaEnUso > POTENCIA_CONTRATADA) {
                int cargaMayor = -1;
                for (int i = 0; i < contadorCargas; i++) {
                    if(estadoCargas[i] && dynamic_cast<CargaPresindible*>(cargas[i]) != nullptr) {
                        if(cargaMayor == -1 || cargas[i]->getpotenciakw() > cargas[cargaMayor]->getpotenciakw()) {
                            cargaMayor = i;
                        }
                    }
                }

                if(cargaMayor != -1) {
                estadoCargas[cargaMayor] = false;
                cout << "ALTO! La potencia en uso *** " << potenciaEnUso << " kW *** es mayor a la contratada "
                << POTENCIA_CONTRATADA << "kW " << endl;
                potenciaEnUso -= cargas[cargaMayor]->getpotenciakw();
                }
            }

            // Aqui se encienden o apagan las cargas dependiendo de los datos arrojados
            // en los ciclos superiores y los valores que se determinaron en ese momento
            for (int i = 0; i < contadorCargas; i++) {
                if(estadoCargas[i]) {
                    cargas[i]->encender();
                } else {
                    cargas[i]->apagar();
                }
            }

            // Se registra el consumo de la hora en el medidor
            medidor.registrarconsumo(potenciaEnUso, precio);
            cout << "Hora: " << hora << " --- Precio: " << precio << " --- Potencia: " << potenciaEnUso << " kW" << endl;


        }
    }


};


int main() {
    int horaInicial = 10;
    float precioInicial = 450;
    float nivelEmbalseInicial = 70;
    bool estadoSimulacion = false;
    Mercado mercado(nivelEmbalseInicial);
    CargaCritica nevera("Nevera de Insumos", POTENCIA_NEVERA);
    CargaPresindible horno("Horno Electrico", POTENCIA_HORNO);
    CargaPresindible amasadora("Amasadora", POTENCIA_AMASADORA);
    Negocio panaderia("Panaderia 24/7", &mercado);
    panaderia.nuevaCarga(&nevera);
    panaderia.nuevaCarga(&horno);
    panaderia.nuevaCarga(&amasadora);
};