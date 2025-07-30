int sensorPin = 34;   // Pin de lectura del potenciómetro
int sensorValue = 0;  // valor del sensor
int resolucion = 1024;
int niveles = 6;

//Funciones
int getLevel(int sensorValue);

void setup() {
  Serial.begin(9600);
  analogReadResolution(10);         // 10 bits (0-1023)
  analogSetPinAttenuation(sensorPin, ADC_11db);
}

void loop() {
  sensorValue = analogRead(sensorPin);
  Serial.print("Valor potenciómetro: ");
  Serial.println(sensorValue);

  Serial.print("Nivel: ");
  Serial.println(getLevel(sensorValue));

  delay(1000);
}

int getLevel(int sensorValue){
  int nivel = resolucion/niveles;
  for(int i = 1; i < niveles; i++){
    if(sensorValue<nivel*i) return i;
  }
  return niveles;
}
