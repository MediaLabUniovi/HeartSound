#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include "configuration.h"
#include "esp_now_functions.h"


uint8_t peer_mac[6];
String nombrePlaca;
uint8_t heartType = CORAZON_1; // tipo de corazón (1-6)

HardwareSerial SerialDFP(2);  // UART1
DFRobotDFPlayerMini myDFPlayer;

// --------------------- VARIABLES --------------------
// Variables de interrupción
volatile heartPosition_t heartPosition = FOCO_AORTICO;   // Posición del altavoz (Consultar Documentación)
volatile bool changeFlag = false;
// Variables de control
bool isPlaying = false;
unsigned long currentMillis = millis();

volatile unsigned long lastInterruptTime1 = 0;
volatile unsigned long lastInterruptTime2 = 0;
volatile unsigned long lastInterruptTime3 = 0;
volatile unsigned long lastInterruptTime4 = 0;
volatile unsigned long lastInterruptTime5 = 0;

const unsigned long debounceDelay_MS = 100; // 100 ms debounce

//Rutinas de atención a la interrupción
void isrPulsador1() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime1 > debounceDelay_MS) {
    heartPosition = FOCO_AORTICO;
    changeFlag = true;
    lastInterruptTime1 = interruptTime;
  }
}

void isrPulsador2() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime2 > debounceDelay_MS) {
    heartPosition = FOCO_PULMONAR;
    changeFlag = true;
    lastInterruptTime2 = interruptTime;
  }
}

void isrPulsador3() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime3 > debounceDelay_MS) {
    heartPosition = FOCO_ACCESORIO_AORTICO;
    changeFlag = true;
    lastInterruptTime3 = interruptTime;
  }
}

void isrPulsador4() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime4 > debounceDelay_MS) {
    heartPosition = FOCO_TRICUSPIDE;
    changeFlag = true;
    lastInterruptTime4 = interruptTime;
  }
}

void isrPulsador5() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime5 > debounceDelay_MS) {
    heartPosition = FOCO_MITRAL;
    changeFlag = true;
    lastInterruptTime5 = interruptTime;
  }
}

//Funciones
void reproducirAudio(int folder, int audio);
void controlAltavoz(int valor);
void apagarAltavoces(void);


void setup() {
  Serial.begin(115200);
  espNOW_setup(); 
  // Configurar relés como salidas
  pinMode(PIN_RELE1, OUTPUT);
  pinMode(PIN_RELE2, OUTPUT);
  pinMode(PIN_RELE3, OUTPUT);
  pinMode(PIN_RELE4, OUTPUT);
  pinMode(PIN_RELE5, OUTPUT);

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

//------------------- Configuración del Reproductor mp3 ------------------
  // Iniciar UART1 en pines (RX) y (TX) a 9600 baudios
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
  myDFPlayer.volume(20);  // Volumen (0-30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // Fuente: tarjeta SD

}

void loop() {
  reproducirAudio(heartType, heartPosition);
  delay(100);
}

/* FUNCIONES */

void reproducirAudio(int folder, int audio){
  if (changeFlag) {
    changeFlag = false;
    //int pin = (audio==1 ? PULSADOR1 : audio==2 ? PULSADOR2: audio==3 ? PULSADOR3: audio==4 ? PULSADOR4 : PULSADOR5);
    int pin;
    switch(audio){
      case 1:
        pin = PULSADOR1;
        break;
      case 2:
        pin = PULSADOR2;
        break;
      case 3:
        pin = PULSADOR3;
        break;
      case 4:
        pin = PULSADOR4;
        break;
      default:
        pin = PULSADOR5;
        break;
    }

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
    if (myDFPlayer.available() && myDFPlayer.readType() == DFPlayerFeedBack) {
      myDFPlayer.playFolder(folder,audio);
    }
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
