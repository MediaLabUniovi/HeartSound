#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include "configuration.h"


// Creamos un objeto Serial1 con pines asignados manualmente
HardwareSerial SerialDFP(2);  // UART1
DFRobotDFPlayerMini myDFPlayer;

/* VARIABLES */
int sensorValue = 0;  // valor del sensor
int nivel;

// Variables de interrupción
volatile int var = 0; // Lado del cuerpo 1-5
volatile bool cambio = false;

// Variables de control
// int carpeta = 0; // Corazón que está sonando 1-6
bool isPlaying = false;
// const unsigned long debounceDelay = 200;   // 50 ms idealmente
// unsigned long lastChangeTime = 0;
unsigned long currentMillis = millis();
// const unsigned long interval = 1000;  // intervalo en milisegundos (aquí 1 s)
// unsigned long previousMillis = 0;     // almacena la última vez que se ejecutó la acción


volatile unsigned long lastInterruptTime1 = 0;
volatile unsigned long lastInterruptTime2 = 0;
volatile unsigned long lastInterruptTime3 = 0;
volatile unsigned long lastInterruptTime4 = 0;
volatile unsigned long lastInterruptTime5 = 0;

const unsigned long debounceDelay = 50; // 50 ms debounce

//Rutinas de atención a la interrupción
void IRAM_ATTR boton1_ISR() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime1 > debounceDelay) {
    var = 1;
    cambio = true;
    lastInterruptTime1 = interruptTime;
  }
}

void IRAM_ATTR boton2_ISR() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime2 > debounceDelay) {
    var = 2;
    cambio = true;
    lastInterruptTime2 = interruptTime;
  }
}

void IRAM_ATTR boton3_ISR() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime3 > debounceDelay) {
    var = 3;
    cambio = true;
    lastInterruptTime3 = interruptTime;
  }
}

void IRAM_ATTR boton4_ISR() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime4 > debounceDelay) {
    var = 4;
    cambio = true;
    lastInterruptTime4 = interruptTime;
  }
}

void IRAM_ATTR boton5_ISR() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime5 > debounceDelay) {
    var = 5;
    cambio = true;
    lastInterruptTime5 = interruptTime;
  }
}

//Funciones
void reproducirAudio(int folder, int audio);
int getLevel(int sensorValue);
void actualizarPines(int valor);
void controlAltavoz(int valor);

void setup() {
  delay(200);
  Serial.begin(115200);

  // configuración ADC
  analogReadResolution(10);         // 10 bits (0-1023)
  analogSetAttenuation(ADC_11db);   // Rango 0-3.3V

  /*
    * Configuración de botones
  */
  // Configurar pines como entradas con pull-up interno
  pinMode(pinBoton1, INPUT_PULLUP);
  pinMode(pinBoton2, INPUT_PULLUP);
  pinMode(pinBoton3, INPUT_PULLUP);
  pinMode(pinBoton4, INPUT_PULLUP);
  pinMode(pinBoton5, INPUT_PULLUP);
  // Asignar interrupciones
  attachInterrupt(digitalPinToInterrupt(pinBoton1), boton1_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinBoton2), boton2_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinBoton3), boton3_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinBoton4), boton4_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinBoton5), boton5_ISR, CHANGE);

  /*
    * Configuración Altavoz
  */
  pinMode(controlAltavoz0, OUTPUT); 
  pinMode(controlAltavoz1, OUTPUT); 
  pinMode(controlAltavoz2, OUTPUT); 
  pinMode(enaAltavoz, OUTPUT);
  digitalWrite(enaAltavoz, HIGH);
  digitalWrite(controlAltavoz0, LOW);
  digitalWrite(controlAltavoz1, LOW);
  digitalWrite(controlAltavoz2, LOW);

  /*
    * Pines LEDs
  */
  pinMode(controlLeds0, OUTPUT); 
  pinMode(controlLeds1, OUTPUT); 
  pinMode(controlLeds2, OUTPUT); 

  digitalWrite(controlLeds0, LOW);
  digitalWrite(controlLeds1, LOW);
  digitalWrite(controlLeds2, LOW);
  

  /* 
    * Configuración del Reproductor mp3
  */
  // Iniciar UART1 en pines 16 (RX) y 25 (TX) a 9600 baudios
  SerialDFP.begin(9600, SERIAL_8N1, RX, TX);
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
  if(!isPlaying){
    sensorValue = analogRead(sensorPin);
    nivel = getLevel(sensorValue);
    actualizarPines(nivel);
    delay(1000);
  }  
  reproducirAudio(nivel, var);
}


/* FUNCIONES */

void reproducirAudio(int folder, int audio){
  if (cambio) {
    cambio = false;

    int pin = (audio==1 ? pinBoton1 : audio==2 ? pinBoton2: audio==3 ? pinBoton3: audio==4 ? pinBoton4 : pinBoton5);

    //mensajes debug
    // Serial.print("Salud corazón: ");
    // Serial.println(folder);
    // Serial.print("Sonido: ");
    // Serial.println(audio);

    controlAltavoz(audio);

    bool pressed = (digitalRead(pin) == LOW); // 1 si está a LOW

    if (pressed && !isPlaying) {
      // Pasamos de HIGH→LOW y no estaba sonando: arrancamos
      myDFPlayer.playFolder(folder,audio);
      Serial.println("Play");
      isPlaying = true;
    }
    else if (!pressed && isPlaying) {
      // Se soltó el botón y estaba sonando: paramos
      myDFPlayer.stop();
      digitalWrite(enaAltavoz, LOW);
      isPlaying = false;
      Serial.println("Pause");
    }
  }
  if (isPlaying){ // El 11 es el fin de canción, cuando llega se vuelve a poner si está el botón pulsado
    if (myDFPlayer.available() && myDFPlayer.readType() == 11) {
      myDFPlayer.playFolder(folder,var);
    }
  }
}


int getLevel(int sensorValue){
  int nivel = RESOLUCION/NIVELES;
  for(int i = 1; i < NIVELES; i++){
    if(sensorValue<nivel*i) return i;
  }
  return NIVELES;
}

void actualizarPines(int valor) {
  valor--;
  if(valor >= 0 && valor < NIVELES){
    digitalWrite(controlLeds0, bitRead(valor, 0)); // Bit 0 para Y0
    digitalWrite(controlLeds1, bitRead(valor, 1)); // Bit 1 para Y1
    digitalWrite(controlLeds2, bitRead(valor, 2)); // Bit 2 para Y2
  }else Serial.println("Error: Led a encender no existe");
}

void controlAltavoz(int valor) {
  valor--;
  if(valor >= 0 && valor < N_ALTAVOCES){
    digitalWrite(controlAltavoz0, bitRead(valor, 0)); // Bit 0 para Y0
    digitalWrite(controlAltavoz1, bitRead(valor, 1)); // Bit 1 para Y1
    digitalWrite(controlAltavoz2, bitRead(valor, 2)); // Bit 2 para Y2
    digitalWrite(enaAltavoz, HIGH);
  }else{
    Serial.println("Error: Altavoz a encender no existe"); return;
  }
}