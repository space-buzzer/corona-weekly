// *** Configuration ***
const int BUZZER = 6;
const int BUTTON_1 = 5;
const int BUTTON_2 = 4;
const int BUTTON_3 = 3;
const int BUTTON_4 = 2;
// ***


const int buttons[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
const int num_buttons = 4;

const int tones[] = {1000, 1200, 1500, 2000};

void setup() {
  pinMode(BUZZER, OUTPUT);
  for (int i = 0; i < num_buttons; i++) {
    pinMode(buttons[i], INPUT);
  }
}

void loop() {
  int button_state = 0;
  bool pressed = false;
  for (int b = 0; b < num_buttons; b++){
    button_state = digitalRead(buttons[b]);
    if (button_state == HIGH) {
      pressed = true;
      tone(BUZZER, tones[b]);
    }
  }

  if (!pressed) {
    noTone(BUZZER);
  }
}
