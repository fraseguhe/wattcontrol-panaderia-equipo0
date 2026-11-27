#ifndef CONSTANTES_H
#define CONSTANTES_H

// Importe de librerias
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

// Directivas
#define HORAS_DIA 24
#define DIAS_SEMANA 7
#define MAX_CARGAS 3
#define SEMILLA 2026

// CONSTANTES DE LA PANADERIA
const float POTENCIA_CONTRATADA = 9.0; // KW maximos para la panaderia
const float PRECIO_MAXIMO = 800;
const float PRECIO_ESCASEZ = 1500.0;   // PRECIO MAXIMO POR KW
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
const int HORAS_MIN_HORNO = 6;
const int HORAS_MIN_AMASADORA = 4;
const int HORA_INICIAL = 10;
const float PRECIO_INICIAL = 450;
const float EMBALSE_INICIAL = 70;
const string NOMBRES_DIAS[DIAS_SEMANA] = {"Lun", "Mar", "Mie", "Jue", "Vie", "Sab", "Dom"};

#endif
