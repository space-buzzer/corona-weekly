// *** Configuration ***
const int BUZZER = 6;
const int LED_R = 7;
const int LED_B = 17;
const int LED_G = 16;

const int BUTTON_1 = 5;
const int BUTTON_2 = 4;
const int BUTTON_3 = 3;
const int BUTTON_4 = 2;

// ***

const int buttons[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
const int num_buttons = 4;

const int LEDS[] = {LED_R, LED_G, LED_B};

// button to color mapping: button index -> color (RGB)
const int mapping[4][3] = {{155, 130, 0}, // yellow
  {0, 0, 255}, // blue
  {255, 0, 0}, // red
  {0, 255, 0}}; // green


const int tones[] = {1000, 1200, 1500, 2000};

// ***


void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
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
      button_pressed(b);
    }
  }

  if (!pressed) {
    button_released();
  }
}

void button_pressed(int bi) {
  // button index bi is pressed
  tone(BUZZER, tones[bi]);
  for (int i = 0; i < 3; i++) {
      digitalWrite(LEDS[i], mapping[bi][i]);      
  }
}

void button_released() {
    noTone(BUZZER);
    for (int i = 0; i < 3; i++) {
      digitalWrite(LEDS[i], LOW);      
    }
}
