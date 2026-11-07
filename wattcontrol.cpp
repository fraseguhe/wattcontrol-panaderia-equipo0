// Importe dde librerias
#include <cmath>
#include <iostream>
#include <string>
using namespace std;

// CONSTANTES DE LA PANADERIA
// KW maximos para la panaderia
const float POTENCIA_CONTRATADA = 15;
// PRECIO MAXIMO POR KW
const float PRECIO_MAXIMO = 1154;
// CAPACIDAD MINIMA DE EMBALSE
const float EMBALSE_MINIMO = 40;

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
// MAIN DE PRUEBA
int main() {
  cout << "=== PRUEBAS DE UNIDAD: MODELO DE CARGAS Y MEDIDOR ===" << endl;

  // Instancias para la Panadería
  CargaCritica horno("Horno Principal", 8.0);              // 8 kW (No se apaga)
  CargaPresindible amasadora("Amasadora Industrial", 4.5); // 4.5 kW (Aplazable)
  Medidor medidor;

  // PRUEBA 1 y 2: Hora pico y energía cara
  int horaPrueba = 19;
  float precioPrueba = 1200.0; // > PRECIO_MAXIMO (1154)
  float embalsePrueba = 50.0;

  cout << "\n[Prueba 1 y 2] Hora: " << horaPrueba << ":00 | Precio: $"
       << precioPrueba << " | Embalse: " << embalsePrueba << "%" << endl;

  cout << horno.getnombre() << " debe encender?: "
       << (horno.debeencender(horaPrueba, precioPrueba, embalsePrueba) ? "SI"
                                                                       : "NO")
       << endl;
  cout << amasadora.getnombre() << " debe encender?: "
       << (amasadora.debeencender(horaPrueba, precioPrueba, embalsePrueba)
               ? "SI"
               : "NO")
       << endl;

  // PRUEBA 3: Escasez de agua (Embalse < 40%)
  embalsePrueba = 35.0;
  cout << "\n[Prueba 3] Embalse al " << embalsePrueba << "% (Bajo critico)"
       << endl;
  cout << amasadora.getnombre() << " debe encender?: "
       << (amasadora.debeencender(10, 800.0, embalsePrueba) ? "SI" : "NO")
       << endl;

  // Prueba de acumulación en Medidor
  medidor.registrarconsumo(horno.getpotenciakw(), 900.0);
  cout << "\n[Medidor] Consumo acumulado: " << medidor.getconsumototalkwh()
       << " kWh | Costo: $" << medidor.getcostototalcop() << " COP" << endl;

  return 0;
}
