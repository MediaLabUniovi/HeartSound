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
volatile int var = 0;
volatile bool cambio = false;

// Variables de control
bool isPlaying = false;
const unsigned long debounceDelay = 500;   // 50 ms idealmente
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

void reproducirAudio(int carpeta, int audio){
  if (carpeta < 1 || carpeta > 99 || audio < 1 || audio > 255) {
    Serial.println("Error: carpeta o audio fuera de rango.");
    return;
  }
  myDFPlayer.playFolder(carpeta, audio); // Reproduce audio.mp3 en carpeta/
  Serial.print("Reproduciendo carpeta ");
  Serial.print(carpeta);
  Serial.print(", audio ");
  Serial.println(audio);
}

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
  attachInterrupt(digitalPinToInterrupt(pinBoton1), boton1_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBoton2), boton2_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBoton3), boton3_ISR, FALLING);
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
    while (true) delay(1000);
    return;
  }
  Serial.println("DFPlayer iniciado correctamente.");
  myDFPlayer.volume(30);  // Volumen (0-30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // Fuente: tarjeta SD
}

void loop() {
  if (cambio) {
    cambio = false;
    
    // Debounce: ignoramos interrupciones muy seguidas
    if (millis() - lastChangeTime < debounceDelay) return;
    lastChangeTime = millis();

    

    int pin = 0;
    switch (var) {
      case 1: pin = pinBoton1; break;
      case 2: pin = pinBoton2; break;
      case 3: pin = pinBoton3; break;
      default: return;
    }

    bool pressed = digitalRead(pin) == LOW;

    Serial.print("Variable: ");
    Serial.println(var);
    Serial.print("Variable: ");
    Serial.println(var);

    if (pressed && !isPlaying) {
      // Pasamos de HIGH→LOW y no estaba sonando: arrancamos
      reproducirAudio(1, var);
      isPlaying = true;
    }
    else if (!pressed && isPlaying) {
      // Se soltó el botón y estaba sonando: paramos
      myDFPlayer.stop();
      isPlaying = false;
      Serial.println("Pausando");
    }
  }
}