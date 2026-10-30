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
#define PIN_RELE1 32
#define PIN_RELE2 33
#define PIN_RELE3 13
#define PIN_RELE4 14
#define PIN_RELE5 22
// Pines UART (DFPlayer)
#define TX 17  // TX del ESP32 → RX del DFPlayer
#define RX 16  // RX del ESP32 ← TX del DFPlayer

/* CONSTANTES */
#define RESOLUCION    1024
#define NIVELES       6
#define N_ALTAVOCES   5

// Arrays de ejemplo
const int reles[N_ALTAVOCES] = {PIN_RELE1, PIN_RELE2, PIN_RELE3, PIN_RELE4, PIN_RELE5};

// Enumeraciones corazones
typedef enum {
  CORAZON_1 = 1,
  CORAZON_2 = 2,
  CORAZON_3 = 3,
  CORAZON_4 = 4,
  CORAZON_5 = 5,
  CORAZON_6 = 6,
} heartType_t;

typedef enum  {
  FOCO_AORTICO = 1,
  FOCO_PULMONAR = 2,
  FOCO_ACCESORIO_AORTICO = 3,
  FOCO_TRICUSPIDE = 4,
  FOCO_MITRAL = 5,
} heartPosition_t;

#endif
