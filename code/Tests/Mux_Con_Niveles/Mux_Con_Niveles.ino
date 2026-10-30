/* PINES */
#define sensorPin A5   // Pin de lectura del potenciómetro
#define Y0_PIN  7
#define Y1_PIN  8
#define Y2_PIN  9

/* CONSTANTES */
#define resolucion 1024
#define niveles 6

/* VARIABLES */
int sensorValue = 0;  // valor del sensor
int nivel;

//Funciones
int getLevel(int sensorValue);
void actualizarPines(int valor);

/*--------------------------------------------------------*/
void setup() {
  Serial.begin(9600);
  pinMode(Y0_PIN, OUTPUT); 
  pinMode(Y1_PIN, OUTPUT); 
  pinMode(Y2_PIN, OUTPUT); 

  digitalWrite(Y0_PIN, 0);
  digitalWrite(Y1_PIN, 0);
  digitalWrite(Y2_PIN, 0);
}
/*--------------------------------------------------------*/
void loop() {
  sensorValue = analogRead(sensorPin);
  nivel = getLevel(sensorValue);

  Serial.print("Nivel: ");
  Serial.println(nivel);

  actualizarPines(nivel);

  delay(1000);
}
/*--------------------------------------------------------*/

/* FUNCIONES */

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
