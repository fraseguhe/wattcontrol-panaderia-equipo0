#include "Negocio.h"
#include "Constantes.h"
#include "ValidacionDeTexto.h"

// Se extrae el menú de main() para que quede limpio, solo crea objetos y llama métodos
void mostrarMenu(string nombreNegocio, string nombreResponsable) {
  cout << endl << "===== WattControl =====" << endl;
  cout << "Negocio: " << nombreNegocio << " | Responsable: " << nombreResponsable << endl;
  cout << "1. Ingresar lectura manual (hora, precio, nivelEmbalse)" << endl;
  cout << "2. Evaluar decision de cada carga" << endl;
  cout << "3. Simular un dia completo (24 h)" << endl;
  cout << "4. Resumen de la ultima simulacion" << endl;
  cout << "5. Simular la semana real" << endl;
  cout << "0. Salir del programa" << endl;
  cout << "Elige una opcion: ";
}

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
    mostrarMenu(nombreNegocio, nombreResponsable);
    cin >> opcionElegida;

    switch (opcionElegida) {
    case 1:
      hora = pedirHora();
      precio = pedirPrecio();
      nivelEmbalse = pedirNivelEmbalse();
      mercado.setNivelEmbalses(nivelEmbalse);
      cout << "Todos los datos están correctos!" << endl;
      break;

    case 2:
      cout << "Decision con los valores iniciales" << endl;
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
        cout << "Empieza una simulacion con la opcion 3 o la 5 antes de usar esta opción." << endl;
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

  return 0;
}