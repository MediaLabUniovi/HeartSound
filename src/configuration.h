#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/* PINOUT */

// Pin de lectura del potenciómetro
#define sensorPin           35

// Control de altavoces
#define controlLeds0        25
#define controlLeds1        26
#define controlLeds2        27

// Pines de los botones (Estos botones son los que activan al pulsar el altavoz)
#define pinBoton1           4
#define pinBoton2           5
#define pinBoton3           18
#define pinBoton4           19
#define pinBoton5           21

// Control de altavoces
#define controlAltavoz0     32
#define controlAltavoz1     33
#define controlAltavoz2     22
#define enaAltavoz          23

// pines UART (SD)
#define TX                  16
#define RX                  17

/* CONSTANTES */
#define RESOLUCION          1024
#define NIVELES             6
#define N_ALTAVOCES         5

#endif