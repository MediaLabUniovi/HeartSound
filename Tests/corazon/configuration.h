#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/* DEFINICIÓN DE PINES */

// Pulsadores con interrupciones
#define PULSADOR1 4
#define PULSADOR2 5
#define PULSADOR3 18
#define PULSADOR4 19
#define PULSADOR5 21

// Relés (control de altavoces)
#define RELE1 32
#define RELE2 33
#define RELE3 13
#define RELE4 14
#define RELE5 22

// Pines UART (DFPlayer)
#define TX 17  // TX del ESP32 → RX del DFPlayer
#define RX 16  // RX del ESP32 ← TX del DFPlayer

/* CONSTANTES */
#define RESOLUCION    1024
#define NIVELES       6
#define N_ALTAVOCES   5

// Arrays de ejemplo
const int reles[N_ALTAVOCES] = {RELE1, RELE2, RELE3, RELE4, RELE5};

#endif
