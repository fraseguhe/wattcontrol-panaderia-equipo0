// Importe dde librerias
#include <cmath>
#include <iostream>
#include <string>
using namespace std;


// Directivas
#define HORAS_DIA 24
#define MAX_CARGAS 3

// CONSTANTES DE LA PANADERIA
// PRECIO MAXIMO POR KW
const float PRECIO_MAXIMO = 800;
// CAPACIDAD MINIMA DE EMBALSE
const float EMBALSE_MINIMO = 40;
// POTENCIA DE CADA CARGA EN EL NEGOCIO
const float POTENCIA_NEVERA = 1.2;
const float POTENCIA_HORNO = 7.0;
const float POTENCIA_AMASADORA = 2.2;
// POTENCIA CONTRATADA
const float POTENCIA_CONTRATADA = 9.0;
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
  // destrcutor para liberar memoria en caso de necesitar destruir cualquier
  // objeto crreado con esta clase o sus clases hijas
  virtual ~Carga() {}
  // se crean metodos para obtener el nombre, la potencia y el estado de
  //  las cargas del sistema bajo un metodo virtual
  virtual bool debeencender(int hora, float preccio, float nivelembalse) = 0;
  string getnombre() const { return nombre; }
  float getpotenciakw() const { return potenciakw; }
  bool estaencendida() const { return encendida; }
  void setencendida(bool estado) { encendida = estado; }
  // Metodos de control de cargas y metodos de calculo copn acumuladores
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
// aca se establece una nueva clase de carga para cosas que no se pueden apagar
//  y que hereda de la clase padre todos sus metodos y argumentos de la clase
//  carga
class CargaCritica : public Carga {
public:
  CargaCritica(string n, float p) : Carga(n, p) {}
  bool debeencender(int hora, float precio, float nivelembalse) override {
    return true;
  }
};
// aca se establece una clase parab equipos o cargas de las que se puede
//  presindir cuando haya picos en el coosto o haya poca agua
class CargaPresindible : public Carga {
public:
  CargaPresindible(string n, float p) : Carga(n, p) {}
  bool debeencender(int hora, float precio, float nivelembalse) override {
    // REGLAS
    bool eshorapico = (hora >= 18 && hora <= 20);
    bool energiacara = (precio > PRECIO_MAXIMO);
    bool escasezagua(nivelembalse < EMBALSE_MINIMO);
    if ((eshorapico && energiacara) || escasezagua) {
      return false;
    }
    return true;
  }
};
// Se crea la clase Medidor que usra Negocio y es la que suma de forma
// acumulada el consumo de energia y su costo total
class Medidor {
private:
  float consumototalkwh;
  float costototalcop;

public:
  Medidor() : consumototalkwh(0), costototalcop(0) {}
  void registarconsumo(float kw, float precio) {
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

// Se define la clase Mercado la cual se usará en Negocio como una relación de asociación,
// Mercado Determina el precio de de la energía a una hora determinada, y también devuelve el nivel
// De los embalses

class Mercado {
private:
    float nivelEmbalses;

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


// Se define Negocio, clase que unifica las relaciones con Carga y Mercado, apartir
// De esta clase se agregan nuevas cargas si hay espacio.

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
    Mercado mercado(EMBALSE_MINIMO);
    CargaCritica nevera("Nevera de Insumos", POTENCIA_NEVERA);
    CargaPresindible horno("Horno Electrico", POTENCIA_HORNO);
    CargaPresindible amasadora("Amasadora", POTENCIA_AMASADORA);
    Negocio panaderia("Panaderia 24/7", &mercado);
    panaderia.nuevaCarga(&nevera);
    panaderia.nuevaCarga(&horno);
    panaderia.nuevaCarga(&amasadora);
};