// main.ino
/*
  * Código de ejemplo de comunicación entre dos módulos ESP
*/

#include "esp_now_functions.h"

uint8_t peer_mac[6];
String nombrePlaca;
uint8_t heartType; // tipo de corazón actual (1-6)

void setup() {
  Serial.begin(115200);
  espNOW_setup();
}

void loop() {
  if (nombrePlaca == "Corazón"){
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      int newHeartType = input.toInt();
      if (newHeartType >= 1 && newHeartType <= 6) {
        heartType = newHeartType;
        Serial.printf("heartType actualizado a %d\n", heartType);
      } else {
        Serial.println("Entrada inválida. Ingresa un número entre 1 y 6.");
      }
    }
  }
  if (nombrePlaca == "Tórax"){
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      if (input.equalsIgnoreCase("ASK")) {
        askForHeartType();
      } else {
        Serial.println("Comando inválido. Escribe 'ASK' para solicitar el tipo de corazón.");
      }
    }
  }
}
