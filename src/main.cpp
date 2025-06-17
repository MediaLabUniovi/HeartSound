#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

#include "configuration.h" // Igual ni hace falta

// Creamos un objeto Serial1 con pines asignados manualmente
HardwareSerial SerialDFP(1);  // UART1
DFRobotDFPlayerMini myDFPlayer;


void setup() {
  Serial.begin(9600);

  /* 
    * Configuración del Reproductor mp3
  */
  // Iniciar UART1 en pines 15 (RX) y 25 (TX) a 9600 baudios
  SerialDFP.begin(9600, SERIAL_8N1, 15, 25);
  delay(3000);
  Serial.println("Iniciando DFPlayer...");
  if (!myDFPlayer.begin(SerialDFP)) {
    Serial.println(" No se pudo comunicar con DFPlayer.");
    Serial.println("Verifica cableado, SD y alimentación.");
    while (true) delay(1000);
    return;
  }
  Serial.println(" DFPlayer iniciado correctamente.");
  myDFPlayer.volume(5);  // Volumen (0-30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // Fuente: tarjeta SD

  
}

void loop() {

}