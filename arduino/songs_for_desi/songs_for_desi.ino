#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "song_scores.h"

#include "FlashStringHelper.h"

# define CHAR_SIZE            4    // TinyFont 4x4

# define GAP_MIN              1
# define GAP_MAX              4

# define TIME_WIDTH           (CHAR_SIZE * 3 + 4)
# define PROGRESS_BAR_HEIGHT  CHAR_SIZE

# define AVATAR_HEIGHT        (HEIGHT - PROGRESS_BAR_HEIGHT - GAP_MAX * 3)
# define AVATAR_WIDTH         AVATAR_HEIGHT

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

const int8_t SONGS_COUNT = 3;

const char MY_BABY_LOVES_TO_GO_POOPY_TITLE[] PROGMEM =
  "MY BABY\nLOVES TO GO\nPOOPY";
const char WASHING_YOUR_FACE_TITLE[] PROGMEM =
  "WASHING\nYOUR FACE";
const char WHOS_MY_LITTLE_BABY_BIRD_TITLE[] PROGMEM =
  "WHO'S MY\nLITTLE BABY\nBIRD";
const char * const SONG_TITLES[] PROGMEM = {
  WASHING_YOUR_FACE_TITLE,
  MY_BABY_LOVES_TO_GO_POOPY_TITLE,
  WHOS_MY_LITTLE_BABY_BIRD_TITLE,
};

const uint16_t SONG_SCORES[] = {
  WASHING_YOUR_FACE_SCORE,
  MY_BABY_LOVES_TO_GO_POOPY_SCORE,
  WHOS_MY_LITTLE_BABY_BIRD_SCORE,
};
const uint16_t SONG_LENGTHS[] = {
  WASHING_YOUR_FACE_LENGTH,
  MY_BABY_LOVES_TO_GO_POOPY_LENGTH,
  WHOS_MY_LITTLE_BABY_BIRD_LENGTH,
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

void drawAvatar(
  int8_t x,
  int8_t y
) {
  // TODO: something! anything!
  arduboy.drawRect(x, y, AVATAR_WIDTH, AVATAR_HEIGHT);
}

void drawPrettyTime(
  int8_t x,
  int8_t y,
  uint16_t millis
) {
  uint16_t seconds = millis / 1000;
  uint16_t minutes = floor(seconds / 60);

  tinyfont.setCursor(x, y);
  tinyfont.print(String(minutes));

  tinyfont.setCursor(x + CHAR_SIZE, y);
  tinyfont.print(":");

  tinyfont.setCursor(x + CHAR_SIZE + 3, y);
  tinyfont.print(
      (seconds < 10 ? "0" : "")
      + String(seconds)
  );
}

void drawText(
  int8_t x,
  int8_t y
) {
  tinyfont.setCursor(x, y);
  tinyfont.print(
    String(trackIndex + 1) + "/" + String(SONGS_COUNT)
  );

  tinyfont.setCursor(x, y + CHAR_SIZE + GAP_MAX);
  tinyfont.print(readFlashStringPointer(&SONG_TITLES[trackIndex]));
}

void drawProgressBar(
  int8_t x,
  int8_t y,

  uint8_t width
) {
  uint8_t rectWidth = width - (TIME_WIDTH + GAP_MIN) * 2;

  drawPrettyTime(
    x, y,
    getElapsedPlayTime()
  );
  drawPrettyTime(
    x + width - TIME_WIDTH, y,
    SONG_LENGTHS[trackIndex]
  );

  arduboy.drawRect(
    x + TIME_WIDTH + GAP_MIN, y,
    rectWidth,
    PROGRESS_BAR_HEIGHT
  );
  arduboy.fillRect(
    x + TIME_WIDTH + GAP_MIN, y,
    rectWidth * float(getElapsedPlayTime()) / SONG_LENGTHS[trackIndex],
    PROGRESS_BAR_HEIGHT
  );
}

void drawDisplay() {
  drawAvatar(GAP_MAX, GAP_MAX);
  drawText(
    GAP_MAX + AVATAR_WIDTH + GAP_MAX,
    GAP_MAX
  );
  drawProgressBar(
    GAP_MAX, HEIGHT - PROGRESS_BAR_HEIGHT - GAP_MAX,
    WIDTH - GAP_MAX * 2
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
