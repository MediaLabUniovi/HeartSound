#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "esp_now_functions.h"

#define sensorPin 34   // Pin de lectura del potenciómetro
#define resolucion 1024
#define niveles 6

//Tira leds
#define LED        23       
#define NUMPIXELS   6

Adafruit_NeoPixel pixels(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);


int sensorValue = 0;  // valor del sensor
int lastNivel = 0;  // valor del sensor
int nivel;

//Funciones
int getLevel(int sensorValue);
void mostrarNivel(int nivel);

uint8_t peer_mac[6];
String nombrePlaca;
uint8_t heartType; // tipo de corazón actual (1-6)

void setup() {
  Serial.begin(115200);

  espNOW_setup();
  
  analogReadResolution(10);         // 10 bits (0-1023)
  analogSetPinAttenuation(sensorPin, ADC_11db);

  // Configuración de pines de salida
  for (int i = 0; i < niveles; i++) {
    pinMode(LED, OUTPUT);
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
  Serial.println("Listo: potenciómetro + LEDs configurados.");
}

void loop() {
  lastNivel = nivel;
  sensorValue = analogRead(sensorPin);
  Serial.print("Valor potenciómetro: ");
  Serial.print(sensorValue);

  Serial.print(", Nivel: ");
  nivel = getLevel(sensorValue);
  Serial.println(nivel);
  mostrarNivel(nivel);

  if(nivel != lastNivel)sendHeartType(nivel);
  delay(500);
}

int getLevel(int sensorValue){
  int nivel = resolucion/niveles;
  for(int i = 1; i < niveles; i++){
    if(sensorValue<nivel*i) return i;
  }
  return niveles;
}

void mostrarNivel(int nivel) {
  for (int i = 0; i < niveles; i++) {
    if (i == nivel - 1) {
      pixels.setPixelColor(i, pixels.Color(50, 0, 0)); // verde
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
  pixels.show();
}
