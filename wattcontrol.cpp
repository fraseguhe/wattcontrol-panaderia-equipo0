// Importe de librerias
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

// Directivas
#define HORAS_DIA 24
#define MAX_CARGAS 3

// CONSTANTES DE LA PANADERIA
const float POTENCIA_CONTRATADA = 15.0; // KW maximos para la panaderia
const float PRECIO_MAXIMO = 800;        // PRECIO MAXIMO POR KW
const float EMBALSE_MINIMO = 40;       // CAPACIDAD MINIMA DE EMBALSE

// POTENCIA DE CADA CARGA EN EL NEGOCIO
const float POTENCIA_NEVERA = 1.2;
const float POTENCIA_HORNO = 7.0;
const float POTENCIA_AMASADORA = 2.2;

// HORAS PICO
const int PICO_INICIO = 18;
const int PICO_FIN = 20;

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
    Mercado* mercado;

public:

    Negocio(string nombre, Mercado* m) : medidor() {
        nombreNegocio = nombre;
        mercado = m;
        contadorCargas = 0;
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