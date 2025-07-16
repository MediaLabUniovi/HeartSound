#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN         23       
#define NUMPIXELS   6

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();    
  pixels.show();     
}

void loop() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0)); // verde
    pixels.show();   
    delay(300);      
  }

  delay(1000); 

  
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
  delay(1000);
}
