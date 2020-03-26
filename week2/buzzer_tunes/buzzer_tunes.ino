#include "music.h"

// *** Configuration ***
const int BUZZER = 4;
const int transition_fraction = 0.95;
const int transition_delta = 0; //50;

// Available tunes
const int SUPER_MARIO = 0;
const int X_FILES = 1;
const int STILL_ALIVE = 2;




void setup() {
  // put your setup code here, to run once:
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  play_music(SUPER_MARIO);
  delay(5000);
}



void play_music(int music_id) {
  // to get the length of the musical piece, we devide by sizeof(int)
  // and then divide by 2, because the structure of music
  // is an array of notes and their durations (both are ints)
  
  if (SUPER_MARIO == music_id) {
      int notes_num = sizeof(super_mario_music) / sizeof(int) / 2;
      play_me(super_mario_music, notes_num, super_mario_tempo);
  } else if (X_FILES == music_id) {
      int notes_num = sizeof(x_files_music) / sizeof(int) / 2;
      play_me(x_files_music, notes_num, x_files_tempo);    
  } else if (STILL_ALIVE == music_id) {
      int notes_num = sizeof(still_alive_music) / sizeof(int) / 2;
      play_me(still_alive_music, notes_num, still_alive_tempo);        
  }
}


void play_me(const int tune[], int tune_len, int tempo) {
  // Calculate what is o (full note) duration is for the given tempo
  // 60 * 1000 is one minute
  unsigned int note_dur = (60 * 1000l) / tempo ;
  unsigned int note_duration = 0;
  int note_dur_fraction = 0;

  for (int note = 0; note < tune_len; note++) {
    note_dur_fraction = tune[2*note + 1];

    // This part is annoying, but it's slightly easier to handle the music
    // when it's in a single array instead of in 2 separate arrays for
    // duration and notes
    if (note_dur_fraction > 0) {
      // multiplying by 4 because (1) is actually 1/4th 
      note_duration = note_dur / note_dur_fraction;
      note_duration *= 4;
    } else if (note_dur_fraction < 0) {
      // keeping it here because I didn't fix all of super-mario tune yet
      note_duration = note_dur / abs(note_dur_fraction);
      note_duration *= 4 * 1.5;
    }

    // play the note
    // There's a transition fraction and delta we can add -- to make transitions between
    // notes crisper
    if (tune[2*note] != REST) {
      tone(BUZZER, tune[2*note], note_duration * transition_fraction - transition_delta);
    }
    delay(note_duration);
    noTone(BUZZER);
  }
}
