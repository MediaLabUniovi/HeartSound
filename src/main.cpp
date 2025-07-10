#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include "configuration.h"


DFRobotDFPlayerMini myDFPlayer;

/* VARIABLES */
int sensorValue = 0;  // valor del sensor
int nivel;

// Variables de interrupción
volatile int var = 0; // Lado del cuerpo 1-5
volatile bool cambio = false;

// Variables de control
bool isPlaying = false, ini = false;
unsigned long currentMillis = millis();

volatile unsigned long lastInterruptTime1 = 0;
volatile unsigned long lastInterruptTime2 = 0;
volatile unsigned long lastInterruptTime3 = 0;
volatile unsigned long lastInterruptTime4 = 0;
volatile unsigned long lastInterruptTime5 = 0;

const unsigned long debounceDelay = 100; // 50 ms debounce

//Rutinas de atención a la interrupción
void isrPulsador1() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime1 > debounceDelay) {
    var = 1;
    cambio = true;
    lastInterruptTime1 = interruptTime;
  }
}

void isrPulsador2() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime2 > debounceDelay) {
    var = 2;
    cambio = true;
    lastInterruptTime2 = interruptTime;
  }
}

void isrPulsador3() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime3 > debounceDelay and ini == true) {
    var = 3;
    cambio = true;
    lastInterruptTime3 = interruptTime;
  }
}

void isrPulsador4() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime4 > debounceDelay and ini == true) {
    var = 4;
    cambio = true;
    lastInterruptTime4 = interruptTime;
  }
}

void isrPulsador5() {
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
void actualizarLeds(int valor);
void controlAltavoz(int valor);
void apagarAltavoces(void);

void setup() {
  delay(200);
  Serial.begin(115200);
  Serial2.begin(9600);     // Comunicación con el  MP3 (en TX2/RX2)

  // Iniciar UART2 en pines 17 (RX) y 16 (TX) a 9600 baudios
  Serial.println("Iniciando DFPlayer...");
  if (!myDFPlayer.begin(Serial2)) {
    Serial.println("No se encontró el DFPlayer Mini.");
    while (true);
  }

  Serial.println("DFPlayer iniciado correctamente.");
  myDFPlayer.volume(30);  // Volumen (0-30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // Fuente: tarjeta SD
 
  // Configurar LEDs como salidas
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);

  delay(500);

  // Configurar relés como salidas
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(RELE3, OUTPUT);
  pinMode(RELE4, OUTPUT);
  pinMode(RELE5, OUTPUT);

  apagarAltavoces();

  // Configurar pulsadores como entrada con pull-up
  pinMode(PULSADOR1, INPUT_PULLUP);
  pinMode(PULSADOR2, INPUT_PULLUP);
  pinMode(PULSADOR3, INPUT_PULLUP);
  pinMode(PULSADOR4, INPUT_PULLUP);
  pinMode(PULSADOR5, INPUT_PULLUP);

  // Configurar interrupciones por flanco de bajada
  attachInterrupt(digitalPinToInterrupt(PULSADOR1), isrPulsador1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PULSADOR2), isrPulsador2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PULSADOR3), isrPulsador3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PULSADOR4), isrPulsador4, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PULSADOR5), isrPulsador5, CHANGE);

  delay(500);
  ini = true;

}

void loop() {
  if(!isPlaying){
    nivel = getLevel(analogRead(POT));
    actualizarLeds(nivel);
    delay(100);
  }  
  reproducirAudio(nivel, var);
}


/* FUNCIONES */

void reproducirAudio(int folder, int audio){
  if (cambio) {
    cambio = false;

    int pin = (audio==1 ? PULSADOR1 : audio==2 ? PULSADOR2: audio==3 ? PULSADOR3: audio==4 ? PULSADOR4 : PULSADOR5);

    //mensajes debug
    Serial.print("Salud corazón: ");
    Serial.println(folder);
    Serial.print("Sonido: ");
    Serial.println(audio);

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
      apagarAltavoces();
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

void actualizarLeds(int valor) {
  valor--; // Convertir de 1–6 a índice 0–5

  if (valor >= 0 && valor < NIVELES) {
    // Apagar todos los LEDs
    for (int i = 0; i < NIVELES; i++) {
      digitalWrite(leds[i], LOW);
    }
    // Encender solo el LED correspondiente
    digitalWrite(leds[valor], HIGH);
  } else {
    Serial.println("Error: Led a encender no existe");
  }
}

void controlAltavoz(int valor) {
  valor--; // Convertir de 1–5 a índice 0–4

  if (valor >= 0 && valor < N_ALTAVOCES) {
    // Apagar todos los relés
    for (int i = 0; i < N_ALTAVOCES; i++) {
      digitalWrite(reles[i], LOW);
    }
    // Activar solo el seleccionado
    digitalWrite(reles[valor], HIGH);
  } else {
    Serial.println("Error: Altavoz a encender no existe");
  }
}

void apagarAltavoces() {
  for (int i = 0; i < N_ALTAVOCES; i++) {
    digitalWrite(reles[i], LOW);
  }
}