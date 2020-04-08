#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "resources.h"

/**
   On Arduino Nano, the SDA (Data) and SCL (Clock) pins
   are mapped like so:
   SDA = A4
   SCL = A5

   This is the default used by Wire library


   Some of the code here, especially initialization is copied from
   Adafruit SSD1306 library example

*/

// *** Configuration ***
const int BUTTON_STOP = 3;
const int BUTTON_START = 4;
const int LED = 5;

#define PRESSED LOW


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// TODO: store in EEPROM
long best_time = -1;


void game_title() {
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Speed"));
  display.setCursor(0, 20);
  display.println(F("Test"));
}

void game_in_game_title() {
  game_title();
  display.drawBitmap(
    display.width() - finger_small_width, 0, 
    finger_small, finger_small_width, finger_small_height, SSD1306_WHITE);

  if (best_time >=0) {
    // 0 shouldn't be possible, but still valid
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(display.width() - finger_small_width, finger_small_height + 1);
    display.println(best_time);
  }
}

void opening_screen() {
  // Clear the buffer on start
  display.clearDisplay();

  int button_finger_spacing_y = 10;
  int button_finger_spacing_x = 10;

  int button_x = (display.width()  - (max(finger_width, button_width)) ) / 2;
  int button_y = (display.height() - (finger_height + button_height + button_finger_spacing_x)) / 2 +
                 finger_height + button_finger_spacing_y;

  int finger_x = (display.width()  - (max(finger_width, button_width)) ) / 2 + button_finger_spacing_x;
  int finger_y = (display.height() - (finger_height + button_height + button_finger_spacing_x)) / 2;


  for (int i = 0; i < 3; i++) {
    display.clearDisplay();

    game_title();
    display.drawBitmap(finger_x, finger_y, finger, finger_width, finger_height, SSD1306_WHITE);
    display.drawBitmap(button_x, button_y, button, button_width, button_height, SSD1306_WHITE);

    display.display();
    delay(500);

    display.clearDisplay();

    game_title();
    display.drawBitmap(finger_x, finger_y + button_finger_spacing_y + 1, finger, finger_width, finger_height, SSD1306_WHITE);
    display.drawBitmap(button_x, button_y, button, button_width, button_height, SSD1306_WHITE);
    display.display();
    delay(500);
  }

}

void report_reaction_screen(long reaction_time) {
  display.clearDisplay();
  game_in_game_title();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 40);
  display.println(F("Reaction Time:"));
  //  display.setCursor(0, 51);

  char data[20];

  if (reaction_time > 1000) {
    // There's no %f in Arduino's print library
    int secs = reaction_time / 1000;
    int mss = reaction_time % 1000;

    sprintf(data, "%d.%04d s", secs, mss);
    display.println(data);
  } else {
    sprintf(data, "%d ms", reaction_time);
    display.println(data);
  }

  display.display();
}

void fail_screen() {
  display.clearDisplay();
  game_in_game_title();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 40);
  display.println(F("Clicked before light went off"));
  display.println(F("FAIL"));

  display.display();  
}

void start_game_screen() {
  display.clearDisplay();
  game_in_game_title();

  display.display();
}

void setup() {
  Serial.begin(9600);

  pinMode(LED, OUTPUT);
  pinMode(BUTTON_STOP, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // show the opening theme
  opening_screen();
  start_game_screen();

}

bool pressed_state = false;
bool light_is_on = false;

void lights_on() {
  digitalWrite(LED, HIGH);
  light_is_on = true;
}

void lights_off() {
  digitalWrite(LED, LOW);
  light_is_on = false;
}

/**
   Starting with the light on
   After a random time:
   - turning the light off,
   - waiting for a button press
   - report the timer
   repeat
*/
void loop() {

  // init round variables
  int wait_with_light = random(10000) + 200;
  bool failed = false;

  Serial.print("--Starting round (");
  Serial.print(wait_with_light);
  Serial.println(")--");
  
  // wait for player to press on the start button
  while(digitalRead(BUTTON_START) != PRESSED) {
    // wait
  }
  Serial.println(" Starting the game");
  start_game_screen();

  long light_started = millis();
  lights_on();
  Serial.print(" lights on: ");
  Serial.println(light_started);
  while ((millis() - light_started) < wait_with_light) {
    int res = digitalRead(BUTTON_START);
    if (digitalRead(BUTTON_START) != PRESSED) {
      failed = true;
      fail_screen();
      break;
    }
  }

  lights_off();
  Serial.println("\n Lights off");

  if (!failed) {
    long reaction_start = millis();
    while (digitalRead(BUTTON_STOP) != PRESSED) {
      // do nothing, just loop
    }
  
    long reaction_time = millis() - reaction_start;
    report_reaction_screen(reaction_time);
    if (best_time < 0 || reaction_time < best_time) {
      best_time = reaction_time;
    }
  }  
  delay(1000);
}
