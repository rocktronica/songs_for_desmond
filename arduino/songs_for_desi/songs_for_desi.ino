#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

#include "graphics.h"
#include "song_lengths.h"
#include "song_scores.h"
#include "song_titles.h"

#include "FlashStringHelper.h"

# define CHAR_SIZE            4    // TinyFont 4x4

# define GAP_MIN              1
# define GAP_MAX              4

# define TIME_WIDTH           (CHAR_SIZE * 3 + 4)
# define PROGRESS_BAR_HEIGHT  CHAR_SIZE

# define AVATAR_HEIGHT        (HEIGHT - PROGRESS_BAR_HEIGHT - GAP_MAX * 3)
# define AVATAR_WIDTH         AVATAR_HEIGHT
# define AVATAR_FRAMES        20

# define FPS                  12

enum Stage {
  Intro,
  Operation
};

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

Stage stage = Intro;
int8_t animationFrame = 0;

// TODO: organize
int8_t trackIndex = 0;
uint16_t trackStartedMillis;
bool isPlaying = false;

void reset() {
  stage = Intro;
  animationFrame = 0;

  trackIndex = 0;

  changeTrack(0);
  arduboyTones.noTone();
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(FPS);

  reset();
}

uint16_t getElapsedPlayTime() {
  if (isPlaying) {
    return millis() - trackStartedMillis;
  }

  return 0;
}

void drawAvatar(
  int8_t x,
  int8_t y
) {
  arduboy.fillRect(x, y, AVATAR_WIDTH, AVATAR_HEIGHT);

  SpritesB::drawOverwrite(
    x + (AVATAR_WIDTH - 35) / 2,
    y + AVATAR_HEIGHT - 40 - 1,
    avatar,
    animationFrame
  );
}

void randomizeAvatar() {
  animationFrame = random(0, AVATAR_FRAMES - 1);
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

void drawIntro() {
  arduboy.fillRect(0, 0, WIDTH, HEIGHT);

  if (animationFrame <= 10) {
    SpritesB::drawOverwrite(
      WIDTH - 92,
      0,
      walk,
      animationFrame
    );
  }

  tinyfont.setCursor(5, 5);
  tinyfont.print("SONGS\nFOR\nDESI");
}

void drawOperation() {
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
  if (trackIndex == newTrackIndex) {
    return;
  }

  trackIndex = newTrackIndex;
  randomizeAvatar();

  if (isPlaying) {
    playCurrentSong();
  }
}

void handleButtonPresses() {
  if (
    arduboy.pressed(A_BUTTON) &&
    arduboy.pressed(B_BUTTON)
  ) {
    reset();
    return;
  }

  if (arduboy.justPressed(A_BUTTON)) {
    isPlaying = false;
  } else if (arduboy.justPressed(B_BUTTON)) {
    isPlaying = true;
    playCurrentSong();
  }

  if (
    arduboy.justPressed(RIGHT_BUTTON) ||
    arduboy.justPressed(UP_BUTTON)
  ) {
    changeTrack((trackIndex + 1) % SONGS_COUNT);
  }else if (
    arduboy.justPressed(LEFT_BUTTON) ||
    arduboy.justPressed(DOWN_BUTTON)
  ) {
    changeTrack(trackIndex > 0 ? trackIndex - 1 : (SONGS_COUNT - 1));
  }
}

void playCurrentSong() {
  arduboyTones.tones(SONG_SCORES[trackIndex]);
  trackStartedMillis = millis();
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();

  if (
    isPlaying &&
    getElapsedPlayTime() >= SONG_LENGTHS[trackIndex]
  ) {
    if (trackIndex < SONGS_COUNT - 1) {
      changeTrack(trackIndex + 1);
    } else if (trackIndex >= SONGS_COUNT - 1) {
      isPlaying = false;
      changeTrack(0);
    }
  }

  if (!isPlaying) {
    arduboyTones.noTone();
  }

  arduboy.clear();

  if (stage == Intro) {
    tinyfont.setTextColor(BLACK);
    drawIntro();

    // TODO: extract/name magic values
    if (arduboy.everyXFrames(FPS / 6)) {
      animationFrame++;
    }

    if (animationFrame > FPS * 1.5) {
      animationFrame = 0;
      stage = Operation;
      tinyfont.setTextColor(WHITE);
    }
  } else {
    handleButtonPresses();

    if (isPlaying) {
      if (arduboy.everyXFrames(FPS / 3)) {
        randomizeAvatar();
      }
    }

    drawOperation();
  }

  arduboy.display();
}
