// Onboard Buzzer simple sketch for Cytron Maker Feather AIoT S3
// Cannot be controlled in V-ONE

const int buzzer = 12;

void setup() {
  pinMode(buzzer, OUTPUT);
}

void loop() {
  tone(buzzer, 1000);
  delay(500);
  tone(buzzer, 1500);
  delay(500);
}
