#include "config.h"
#include <avr/sleep.h>

/**
 * The Music Memory Game
 * 
 * A simple game that is played in rounds, in every round
 * the device plays a tune and the player needs to repeat it.
 * If the player repeats the tune successfully, the game continues
 * to the next round, with a longer tune.
 * 
 * 
 * The game uses 4 push buttons, 4 LEDs and a buzzer.
 * 
 * Instructions:
 * - The goal of the game is to correctly repeat the sequence
 *   of tones the Arduino is playing
 * - A single game consists of several rounds, each round with
 *   increased difficulty -- 1st round starts with 1 tone,
 *   2nd round with 2 tones and so on all the way to 20 
 *   (configurable max. rounds).
 * - The player has 1 second (configurable).
 * 
 */


// change this to HIGH if using external pullup
#define PRESSED LOW


// initial state for global variables
int cur_round = 0;
bool winning = true;

void test() {
  int button_state;
  while (true) {
    bool pressed = false;
    for (int b = 0; b < num_buttons; b++) {
      button_state = digitalRead(buttons[b]);
      if (button_state == PRESSED) {
        button_pressed(b);
        pressed = true;
      }
    }
    if (!pressed) {
      button_released();  
    }
  }
}

void setup_game() {
  // wait for the player to start the game 
  long start_press = 0;
  bool start_game = false;
  bool button_pressed = false;

  int i = 0; // the button we need to press to start the game
  
  while (!start_game) {
    // loop forever until we can start the game  
    int button_state = 0;
    for (int i = 0; i < num_buttons; i++) {
      button_state = digitalRead(buttons[i]);
      if (button_state == PRESSED) {
        // stop on the 1st button that's pressed
        break;
      }
    }
    if (button_state == PRESSED && !button_pressed) {
      button_pressed = true;
      start_press = millis(); // start counting time
    } else if (button_state != PRESSED) {
        if (button_pressed) {
          // button was pressed and lifted
          if ((millis() - start_press) < game_start_press_duration) {
            Serial.println("Starting game press too short");
          }        
        }
      
        button_pressed = false;
                  
        // if we lifted the button, check whether it was pressed
        // for the required duration
        if ((millis() - start_press) > game_start_press_duration) {
          // we can star the game
          start_game = true;
        } else {
          start_press = millis();
        }
    }
  }

  Serial.println("Starting the game");
  // get the random seed
  randomSeed(analogRead(0));

  // reset all values
  cur_round = 0;
  winning = true;
}


void setup() {
  pinMode(BUZZER, OUTPUT);

  for (int i = 0; i < 3; i++) {
    pinMode(leds[i], OUTPUT);
  }
  for (int i = 0; i < num_buttons; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }

  Serial.begin(9600);

  // Instead of starting the game, uncomment the `test` function here
  // to help with initial setup and color to button assignment.
  // This function is blocking, the name will never start
  // test();

  // Start the game
  setup_game();
  
}


/**
 * The player needs to repeat cur_round tones tones from the tune[]
 * 
 * Returns true: if player won the round, false if lost
 */
bool player_round(const int cur_round, const int tune[]) {
  long start_timer = 0;  // time, millis

  Serial.print("Player round ");
  Serial.println(cur_round);

  Serial.print("   Player tones: ");

  for (int i = 0; i <= cur_round; i++) {    
    Serial.print(", ");
    start_timer = millis();
    bool tone_successful = false; // if current tone was successful
    bool printed_round = false;
    
    // wait for button press
    while ((millis() - start_timer) < game_wait_for_press_duration) {
      int button_state = 0;
      bool pressed = false;
      for (int b = 0; b < num_buttons; b++) {
        button_state = digitalRead(buttons[b]);
        if (button_state == PRESSED) {

          if (!printed_round) {
            // print only on first press
            printed_round = true;
            Serial.print(b);
          }
          
          pressed = true;

          button_pressed(b);
          if (b == tune[i]) {            
            tone_successful = true;
          } else {
            // this part is probably not needed, since there will be 
            // a button release event anyways
            tone_successful = false;
          }
        }
      }

      if (!pressed) {
        button_released();
        if (tone_successful) {
          break;
        }
      } 
      
    } // while: waiting for single button press

    if (!tone_successful) {
      Serial.println("");
      Serial.println("  Return FALSE for round");
      return false;
    }

  } // for: all tones in tune

  Serial.println("");
  Serial.println("  Return TRUE for round");
  return true;
}


void loop() {
  int tune[max_rounds];
  
  while (winning && cur_round < max_rounds) {
    // set next tone
    tune[cur_round] = random(num_buttons);

    // play the full tune until cur_round
    for (int i = 0; i <= cur_round; i++) {
      button_pressed(tune[i]);
      delay(tone_duration);
      button_released();
      delay(tone_pause);
    }

    // wait for player to repeat the tune
    delay(500);
    winning = player_round(cur_round, tune);
    Serial.print(" Player ");
    if (winning) {
      Serial.println("is WINNING");
    } else {
      Serial.println("LOST");
      end_of_game();
    }
    
    cur_round++;

    delay(round_delay_duration);    
  }
  button_released();

  if (!winning) {
    sleep_mode();
  }

}

void button_pressed(int bi) {
  // button index bi is pressed
  tone(BUZZER, tones[bi]);
  for (int i = 0; i < 3; i++) {
      digitalWrite(leds[i], mapping[bi][i]);      
  }
}

void button_released() {
    noTone(BUZZER);
    for (int i = 0; i < 3; i++) {
      digitalWrite(leds[i], LOW);      
    }
}

void start_of_game() {
  // TODO: play start of game sound, flash LEDs
}

void end_of_game() {
  int melody[] = {
    262, 196, 196, 220, 196, 0, 247, 262
  };
  
  int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};
  
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(15, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(15);
  }
}
