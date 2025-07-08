int Y0_PIN  = 7;
int Y1_PIN  = 8;
int Y2_PIN  = 9;

bool Level = HIGH;

void setup() {
  // put your setup code here, to run once:
  pinMode(Y0_PIN, OUTPUT); 
  pinMode(Y1_PIN, OUTPUT); 
  pinMode(Y2_PIN, OUTPUT); 
  digitalWrite(Y0_PIN, 1);
  digitalWrite(Y1_PIN, 0);
  digitalWrite(Y2_PIN, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Level = !Level;
  // digitalWrite(Y1_PIN, Level);

}
