int BUZZER = 3;

void setup() {
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  tone(BUZZER, 8000); // output pin, frequency
  delay(500);
  noTone(BUZZER);
  delay(2000);
}
