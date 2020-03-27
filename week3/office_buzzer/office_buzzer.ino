#include <Encoder.h>

// *** Configuration ***
const int BUZZER = 4;
const int KNOB_A = 5;
const int KNOB_B = 6;
const int KNOB_SWT = 7;

const int FREQ = 1750; // frequency of the beep
const int STEP = 10; // loop interval, keep it low
const int BEEP_DURATION = 100;

enum STATE {
  STATE_A,
  STATE_B,
};

Encoder knob(KNOB_A, KNOB_B);
STATE device_state = STATE_A;

void setup() {
  pinMode(BUZZER, OUTPUT);

  pinMode(KNOB_A, INPUT);
  pinMode(KNOB_B, INPUT);
  pinMode(KNOB_SWT, INPUT);

  // for textual output
  Serial.begin(9600); 
}


int interval = 2000; // initial interval
int tick = 0;
long knob_state = 0; // state of the knob


void loop() {
  handle_switch();
  handle_knob();  
  handle_beep(device_state);
}

int switch_state = LOW;
long debounce = 300; // in millis. Emit only one state within this interval
long switch_time = 0;

void handle_switch() {
  // read the switch
  switch_state = digitalRead(KNOB_SWT);
  if (switch_state == LOW and (millis() - switch_time) > debounce ) {
    // pressed
    Serial.println("switch pressed");
    switch_time = millis();

    // toggle device state
    if (device_state == STATE_A) {
      device_state = STATE_B;
    } else {
      device_state = STATE_A;
    }    
  }
}

void handle_knob() {
    // read the knob
  long currentState = knob.read();
  if (currentState != knob_state) {
    if (currentState < knob_state) {
      interval -= 50;
    } else {
      interval += 50;
    }
    knob_state = currentState;
    Serial.println(knob_state);
  }
}

void handle_beep(STATE dev_state) {
  // make a sound only in STATE_A
  if (dev_state == STATE_A && (tick == 0 || tick >= interval)) {
    // buzz me
    tone(BUZZER, FREQ);
    delay(100);
    noTone(BUZZER);
    tick = (tick + 100) % interval;
  } else {
    delay(STEP);    
    tick = (tick + STEP) % interval;
  }
}
