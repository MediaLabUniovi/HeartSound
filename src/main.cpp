#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

//#include "configuration.h" // Igual ni hace falta

// Creamos un objeto Serial1 con pines asignados manualmente
HardwareSerial SerialDFP(2);  // UART1
DFRobotDFPlayerMini myDFPlayer;

// Pines de los botones
const int pinBoton1 = 17;
const int pinBoton2 = 18;
const int pinBoton3 = 19;

// Variables de interrupción
int carpeta = 0;
volatile int var = 0;
volatile bool cambio = false;

// Variables de control
bool isPlaying = false;
const unsigned long debounceDelay = 100;   // 50 ms idealmente
unsigned long lastChangeTime = 0;

// Rutinas de interrupción
void IRAM_ATTR boton1_ISR() {
  var = 1;
  cambio = true;
}
void IRAM_ATTR boton2_ISR() {
  var = 2;
  cambio = true;
}
void IRAM_ATTR boton3_ISR() {
  var = 3;
  cambio = true;
}

void reproducirAudio(int folder, int audio);

void setup() {
  Serial.begin(115200);
  /*
    * Configuración de botones
  */
  // Configurar pines como entradas con pull-up interno
  pinMode(pinBoton1, INPUT_PULLUP);
  pinMode(pinBoton2, INPUT_PULLUP);
  pinMode(pinBoton3, INPUT_PULLUP);
  // Asignar interrupciones
  attachInterrupt(digitalPinToInterrupt(pinBoton1), boton1_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinBoton2), boton2_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinBoton3), boton3_ISR, CHANGE);
  Serial.println("Interrupciones configuradas.");

  /* 
    * Configuración del Reproductor mp3
  */
  // Iniciar UART1 en pines 16 (RX) y 25 (TX) a 9600 baudios
  SerialDFP.begin(9600, SERIAL_8N1, 16, 25);
  delay(1000);
  Serial.println("Iniciando DFPlayer...");
  if (!myDFPlayer.begin(SerialDFP)) {
    Serial.println("No se pudo comunicar con DFPlayer.");
    Serial.println("Verifica cableado, SD y alimentación.");
    while (true);
    return;
  }
  Serial.println("DFPlayer iniciado correctamente.");
  myDFPlayer.volume(30);  // Volumen (0-30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // Fuente: tarjeta SD
}

void loop() {
  reproducirAudio(carpeta, var);
}

void reproducirAudio(int folder, int audio){
  if (cambio) {
    cambio = false;
    // Debounce: ignoramos interrupciones muy seguidas
    if (millis() - lastChangeTime < debounceDelay) return;
    lastChangeTime = millis();
    int pin = (var==1 ? pinBoton1 : var==2 ? pinBoton2 : pinBoton3);
    bool pressed = (digitalRead(pin) == LOW); // 1 si está a LOW
    
    if (pressed && !isPlaying) {
      // Pasamos de HIGH→LOW y no estaba sonando: arrancamos
      myDFPlayer.playFolder(carpeta,var);
      Serial.printf("Bucle carpeta %d, audio %d\n", 1, var);
      isPlaying = true;
    }
    else if (!pressed && isPlaying) {
      // Se soltó el botón y estaba sonando: paramos
      myDFPlayer.stop();
      isPlaying = false;
      Serial.println("Pausando");
    }
  }
  if (isPlaying){ // El 11 es el fin de canción, cuando llega se vuelve a poner si está el botón pulsado
    if (myDFPlayer.available() && myDFPlayer.readType() == 11) {
    myDFPlayer.playFolder(carpeta,var);
    }
  }
}