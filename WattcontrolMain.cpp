#include "Negocio.h"
#include "Constantes.h"
#include "ValidacionDeTexto.h"

int main() {
  srand(SEMILLA);
  string nombreNegocio = pedirTexto("Nombre del negocio: ");
  string nombreResponsable = pedirTexto("Nombre del responsable: ");
  int hora = HORA_INICIAL;
  int opcionElegida;
  float precio = PRECIO_INICIAL;
  float nivelEmbalse = EMBALSE_INICIAL;
  bool estadoSimulacion = false;
  Mercado mercado(nivelEmbalse);
  CargaCritica nevera("Nevera de Insumos", POTENCIA_NEVERA);
  CargaAplazable horno("Horno Electrico", POTENCIA_HORNO, HORAS_MIN_HORNO);
  CargaAplazable amasadora("Amasadora", POTENCIA_AMASADORA, HORAS_MIN_AMASADORA);
  Negocio panaderia(nombreNegocio, &mercado);
  panaderia.nuevaCarga(&nevera);
  panaderia.nuevaCarga(&horno);
  panaderia.nuevaCarga(&amasadora);

  do {
    cout << endl << "===== WattControl =====" << endl;
    cout << "Negocio: " << nombreNegocio << " | Responsable: " << nombreResponsable << endl;
    cout << "1. Ingresar lectura manual (hora, precio, nivelEmbalse)" << endl;
    cout << "2. Evaluar decision de cada carga" << endl;
    cout << "3. Simular un dia completo (24 h)" << endl;
    cout << "4. Resumen de la ultima simulacion" << endl;
    cout << "5. Simular la semana real" << endl;
    cout << "0. Salir del programa" << endl;
    cout << "Elige una opcion: ";
    cin >> opcionElegida;

    switch (opcionElegida) {
    case 1:
      do {
        cout << "Hora: ";
        cin >> hora;
        if (hora < 0 || hora > 23) {
          cout << " Hora invalidad, intenta nuevamente (Entero entre 0 y 23)" << endl;
        }
      } while (hora < 0 || hora > 23);

      do {
        cout << "Precio en bolsa: ";
        cin >> precio;

        if (precio <= 0) {
          cout << " Error, valor invalido, el precio debe ser mayor a 0" << endl;
        }
      } while (precio <= 0);

      do {
        cout << "Cual es el nivel de los Embalses: ";
        cin >> nivelEmbalse;

        if (nivelEmbalse < 0 || nivelEmbalse > 100) {
          cout << " Los valores de los embalses deben estar entre 0 y 100" << endl;
        }
      } while (nivelEmbalse < 0 || nivelEmbalse > 100);

      mercado.setNivelEmbalses(nivelEmbalse);

      cout << "Ingreso de datos exitoso!" << endl;
      break;

    case 2:
      cout << "Desicion con los datos iniciales" << endl;
      panaderia.evaluarDecision(hora, precio, nivelEmbalse);
      break;

    case 3:
      panaderia.simularDia();
      estadoSimulacion = true;
      break;

    case 4:
      if (estadoSimulacion) {
        panaderia.resumen();
      }
      else {
        cout << "Inicialice una simulacion con la opcion 3 antes de usar esta opcion." << endl;
      }
      break;
    
    case 5:
      panaderia.simularSemana();
      estadoSimulacion = true;      
      break;

    case 0:
      cout << "Apagando equipos... Adios!" << endl;
      break;

    default:
      cout << "Opcion elegida no valida, intenta nuevamente" << endl;
    }
  } while (opcionElegida != 0);
};
