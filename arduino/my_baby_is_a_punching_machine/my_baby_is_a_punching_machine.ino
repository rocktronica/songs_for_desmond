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
// So these are pointers I guess?
const uint16_t SONG_SCORES[] = {
  WASHING_YOUR_FACE_SCORE,
  MY_BABY_LOVES_TO_GO_POOPY_SCORE,
};

const String TEST_STUFF[] = {
  "LOREM",
  "IPSUM DOLOR"
};

int8_t trackIndex = 0;
uint32_t trackStartedMillis;
uint32_t millisPlayed = 0;

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);
}

uint32_t getSongLength(const uint16_t *tones) {
  uint8_t total = 0;

  // Smaller var types cause infinite loop. Endpoint not right?
  for (int16_t i = 0; &tones[i] != TONES_END; i++) {
    if (i % 2 == 1) {
      total += &tones[i];
    }
  }

  // return total;
  return &tones[0];
}

uint16_t getElapsedPlayTime() {
  if (arduboyTones.playing()) {
    millisPlayed = millis() - trackStartedMillis;
  }

  return millisPlayed;
}

void drawDisplay() {
  tinyfont.setCursor(1, 1);
  tinyfont.print(readFlashStringPointer(&SONG_TITLES[trackIndex]));

  tinyfont.setCursor(1, 11);
  tinyfont.print(trackIndex);

  tinyfont.setCursor(20, 11);
  // tinyfont.print(getSongLength(SONG_SCORES[trackIndex]));
  // tinyfont.print(MY_BABY_LOVES_TO_GO_POOPY_SCORE[7]); // works
  // tinyfont.print(&SONG_SCORES[trackIndex]); // why not?
  // SONG_SCORES[0 and 1] are 172, rest are 0
  tinyfont.print(pgm_read_word(SONG_SCORES[trackIndex]));
  // pgm_read_ptr does nothing
  // time for bed!

  // why is the current score not an array?
  // uint16_t wtf[] = SONG_SCORES[trackIndex];
  // tinyfont.print(wtf);
  // confused why it works for arduboyTones but not this
  // OH!: bc ^ has inProgmem and pgm_read_word

  tinyfont.setCursor(51, 11);
  tinyfont.print(TEST_STUFF[trackIndex][0]);

  tinyfont.setCursor(1, 21);
  tinyfont.print(getElapsedPlayTime());
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
