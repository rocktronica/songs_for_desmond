#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "songs.h"

#include "FlashStringHelper.h"

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

const int8_t SONGS_COUNT = 1;
const char * const SONG_TITLES[] PROGMEM {
  WASHING_YOUR_FACE_TITLE
};

int8_t songIndex = 0;

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);
}

void drawDisplay() {
  tinyfont.setCursor(1, 1);
  tinyfont.print(readFlashStringPointer(&SONG_TITLES[songIndex]));
}

void handleButtonPresses() {
  if (arduboy.justPressed(A_BUTTON)) {
    arduboyTones.noTone();
  } else if (arduboy.justPressed(B_BUTTON)) {
    arduboyTones.tones(WASHING_YOUR_FACE_SCORE);
  }

  if (arduboy.justPressed(RIGHT_BUTTON)) {
    songIndex = (songIndex + 1) % SONGS_COUNT;
  }else if (arduboy.justPressed(LEFT_BUTTON)) {
    songIndex = songIndex > 0 ? songIndex - 1 : (SONGS_COUNT - 1);
  }
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  handleButtonPresses();

  arduboy.clear();
  drawDisplay();
  arduboy.display();
}
