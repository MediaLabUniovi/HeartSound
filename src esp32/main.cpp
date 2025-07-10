#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

/* PINES */
// Pin de lectura del potenciómetro
#define sensorPin A5  // TODO: Cambiar por el del ESP32
// Control de altavoces
#define Y0_PIN  7 
#define Y1_PIN  8
#define Y2_PIN  9

/* CONSTANTES */
#define resolucion 1024
#define niveles 6

// Creamos un objeto Serial1 con pines asignados manualmente
HardwareSerial SerialDFP(2);  // UART1
DFRobotDFPlayerMini myDFPlayer;

// Pines de los botones (Estos botones son los que activan al pulsar el altavoz)
// TODO: Añadir botones
const int pinBoton1 = 17;
const int pinBoton2 = 18;
const int pinBoton3 = 19;

/* VARIABLES */
int sensorValue = 0;  // valor del sensor
int nivel;

// Variables de interrupción
volatile int var = 0; // Lado del cuerpo 1-5
volatile bool cambio = false;

// Variables de control
int carpeta = 0; // Corazón que está sonando 1-6
bool isPlaying = false;
const unsigned long debounceDelay = 100;   // 50 ms idealmente
unsigned long lastChangeTime = 0;
unsigned long currentMillis = millis();
const unsigned long interval = 1000;  // intervalo en milisegundos (aquí 1 s)
unsigned long previousMillis = 0;     // almacena la última vez que se ejecutó la acción

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

//Funciones
void reproducirAudio(int folder, int audio);
int getLevel(int sensorValue);
void actualizarPines(int valor);

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
    * Configuración mando de CONFIG_TWAI_ERRATA_FIX_TX_INTR_LOST
  */
  pinMode(Y0_PIN, OUTPUT); 
  pinMode(Y1_PIN, OUTPUT); 
  pinMode(Y2_PIN, OUTPUT); 
  digitalWrite(Y0_PIN, 0);
  digitalWrite(Y1_PIN, 0);
  digitalWrite(Y2_PIN, 0);

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
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { // Cada segundo mira que nivel se está mandando desde el mando 
    previousMillis = currentMillis;
    sensorValue = analogRead(sensorPin);
    nivel = getLevel(sensorValue);
    Serial.print("Nivel: ");
    Serial.println(nivel);
    actualizarPines(nivel);
  }
  reproducirAudio(nivel, var);
}


/* FUNCIONES */

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

int getLevel(int sensorValue){
  int nivel = resolucion/niveles;
  for(int i = 1; i < niveles; i++){
    if(sensorValue<nivel*i) return i;
  }
  return niveles;
}

void actualizarPines(int valor) {
  valor--;
  if(valor >= 0 && valor < niveles){
    digitalWrite(Y0_PIN, bitRead(valor, 0)); // Bit 0 para Y0
    digitalWrite(Y1_PIN, bitRead(valor, 1)); // Bit 1 para Y1
    digitalWrite(Y2_PIN, bitRead(valor, 2)); // Bit 2 para Y2
  }else Serial.println("Error: Led a encender no existe");
}