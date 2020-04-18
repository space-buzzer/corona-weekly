// *** Configuration ***

const int BUZZER = 15;
const int LED_R = 6;
const int LED_G = 17;
const int LED_B = 16;

const int BUTTON_1 = 5;
const int BUTTON_2 = 4;
const int BUTTON_3 = 3;
const int BUTTON_4 = 2;

// ***

// The order of the buttons sets the order of the tones
// and order of colors
const int buttons[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
const int num_buttons = 4;

const int leds[] = {LED_R, LED_G, LED_B};

// button to color mapping: button index -> color (RGB)
const int mapping[4][3] = {{250, 2, 0}, // yellow
  {0, 0, 255}, // blue
  {255, 0, 0}, // red
  {0, 255, 0}}; // green


// *** Game Settings ***
const int max_rounds = 20;
const int tones[] = {1047, 1319, 1568, 1760};

// Durations
const long tone_duration = 500;
const long tone_pause = 100;
const long round_delay_duration = 1200;

const long game_start_press_duration = 1000;
const long game_wait_for_press_duration = 1000;
