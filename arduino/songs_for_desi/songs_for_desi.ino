#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "songs.h"

#include "FlashStringHelper.h"

# define CHAR_SIZE            4    // TinyFont 4x4
# define PROGRESS_BAR_HEIGHT  10

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
const uint16_t SONG_LENGTHS[] = {
  WASHING_YOUR_FACE_LENGTH,
  MY_BABY_LOVES_TO_GO_POOPY_LENGTH,
};

int8_t trackIndex = 0;
uint16_t trackStartedMillis;
uint16_t millisPlayed = 0;

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

void drawCenteredText(
  String text,
  int8_t y,
  uint8_t lineIndex = 0
) {
  tinyfont.setCursor(
    (WIDTH - (CHAR_SIZE * text.length() + (text.length() - 1))) / 2,
    y + (CHAR_SIZE + 1) * lineIndex
  );
  tinyfont.print(text);
}

String getPrettyTime(uint16_t millis) {
  uint16_t seconds = millis / 1000;
  uint16_t minutes = floor(seconds / 60);

  return String(minutes) + ":"
    + (seconds < 10 ? "0" : "")
    + String(seconds);
}

void drawText(int8_t y) {
  drawCenteredText(
    String(trackIndex + 1) + "/" + String(SONGS_COUNT),
    y, 0
  );
  drawCenteredText(
    readFlashStringPointer(&SONG_TITLES[trackIndex]),
    y, 1
  );

  drawCenteredText("DADA", y, 2);
  drawCenteredText("SONGS FOR DESI", y, 4);
}

void drawProgressBar(
  int8_t x,
  int8_t y,

  uint8_t width
) {
  tinyfont.setCursor(x, y);
  tinyfont.print(getPrettyTime(getElapsedPlayTime()));
  tinyfont.setCursor(x + width - (4 * 4 + 3), y);
  tinyfont.print(getPrettyTime(SONG_LENGTHS[trackIndex]));

  arduboy.drawRect(x, y + 4 + 1, width, PROGRESS_BAR_HEIGHT);
  arduboy.fillRect(
    x, y + 4 + 1,
    width * float(getElapsedPlayTime()) / SONG_LENGTHS[trackIndex], PROGRESS_BAR_HEIGHT
  );
}

void drawDisplay() {
  uint8_t progressBarY = HEIGHT - (CHAR_SIZE + 1 + PROGRESS_BAR_HEIGHT);

  drawText((progressBarY - (4 * 5 + (5 - 1))) / 2);
  drawProgressBar(
    0, progressBarY,
    WIDTH
  );
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
