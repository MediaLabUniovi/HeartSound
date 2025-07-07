#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/* PINOUT */
// --- Definición de pines ---

// LEDs
#define LED1 22
#define LED2 23
#define LED3 24
#define LED4 25
#define LED5 26
#define LED6 27

// Pulsadores con interrupciones
#define PULSADOR1 2
#define PULSADOR2 3
#define PULSADOR3 18
#define PULSADOR4 19
#define PULSADOR5 20

// Relés
#define RELE1 28
#define RELE2 29
#define RELE3 30
#define RELE4 31
#define RELE5 32

// Potenciómetro
#define POT A0

// pines UART (SD)
#define TX                  16
#define RX                  17

/* CONSTANTES */
#define RESOLUCION          1024
#define NIVELES             6
#define N_ALTAVOCES         5

const int leds[NIVELES] = {LED1, LED2, LED3, LED4, LED5, LED6};
const int reles[N_ALTAVOCES] = {RELE1, RELE2, RELE3, RELE4, RELE5};


#endif