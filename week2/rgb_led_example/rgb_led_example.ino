/**
 * RGB LED test example
 */

// *** Configuration ***
const int LED_R = 3;
const int LED_G = 4;
const int LED_B = 5;
// ***

int leds[] = {LED_R, LED_G, LED_B};

void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  // flash leds in order
  for (int i = 0; i < 3; i++) {
    digitalWrite(leds[i], HIGH);
    delay(500);
    digitalWrite(leds[i], LOW);
  }
  delay(1000);
}
