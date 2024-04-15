#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "songs.h"

#include "FlashStringHelper.h"

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

const int8_t SONGS_COUNT = 2;
const char * const SONG_TITLES[] PROGMEM = {
  WASHING_YOUR_FACE_TITLE,
  MY_BABY_LOVES_TO_GO_POOPY_TITLE,
};
const uint16_t SONG_SCORES[] = {
  WASHING_YOUR_FACE_SCORE,
  MY_BABY_LOVES_TO_GO_POOPY_SCORE,
};
const uint32_t SONG_LENGTHS[] = {
  WASHING_YOUR_FACE_LENGTH,
  MY_BABY_LOVES_TO_GO_POOPY_LENGTH,
};

int8_t trackIndex = 0;
uint32_t trackStartedMillis;
uint32_t millisPlayed = 0;

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);
}

uint16_t getElapsedPlayTime() {
  if (arduboyTones.playing()) {
    millisPlayed = millis() - trackStartedMillis;
  } else if (millisPlayed > 0) {
    millisPlayed = SONG_LENGTHS[trackIndex];
  }

  return millisPlayed;
}

void drawDisplay() {
  tinyfont.setCursor(1, 1);
  tinyfont.print(readFlashStringPointer(&SONG_TITLES[trackIndex]));

  tinyfont.setCursor(1, 11);
  tinyfont.print(float(getElapsedPlayTime()) / SONG_LENGTHS[trackIndex]);
}

void changeTrack(int8_t newTrackIndex) {
  trackIndex = newTrackIndex;

  trackStartedMillis = millis();
  millisPlayed = 0;

  if (arduboyTones.playing()) {
    arduboyTones.tones(SONG_SCORES[trackIndex]);
  }
}

void handleButtonPresses() {
  if (arduboy.justPressed(A_BUTTON)) {
    arduboyTones.noTone();

    millisPlayed = 0;
  } else if (arduboy.justPressed(B_BUTTON)) {
    arduboyTones.tones(SONG_SCORES[trackIndex]);

    trackStartedMillis = millis();
    millisPlayed = 0;
  }

  if (arduboy.justPressed(RIGHT_BUTTON)) {
    changeTrack((trackIndex + 1) % SONGS_COUNT);
  }else if (arduboy.justPressed(LEFT_BUTTON)) {
    changeTrack(trackIndex > 0 ? trackIndex - 1 : (SONGS_COUNT - 1));
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
