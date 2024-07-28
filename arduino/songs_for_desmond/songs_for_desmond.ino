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
# define INTRO_SECONDS        1.5
# define INTRO_FRAMES         10
# define INTRO_FRAMERATE      (FPS / 6)
# define AVATAR_FRAMERATE     (FPS / 3)

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

enum Stage {
  Intro,
  Operation
};

struct State {
  Stage stage = Intro;
  int8_t animationFrame = 0;

  int8_t trackIndex = 0;
  uint16_t trackStartedMillis;

  bool isPlaying = false;
} state;

// Songs are songs; tracks are ordered songs
const int8_t TRACKS[] = {
  WHOS_MY_LITTLE_BABY_BIRD,
  ANY_KIND_OF_CAR_AND_ANY_KIND_OF_TRUCK,
  WERE_ALMOST_HOME,
  MY_BABY_LOVES_TO_GO_POOPY,
  WASHING_YOUR_FACE,
  WHOS_MY_LITTLE_DAIKINI_BABY,
  THIS_TOWNS_GOT_A_LOT_OF_CONSTRUCTION,
};

void reset() {
  state.stage = Intro;
  state.animationFrame = 0;

  state.isPlaying = false;

  changeTrack(0);
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(FPS);

  reset();
}

uint16_t getElapsedPlayTime() {
  if (state.isPlaying) {
    return millis() - state.trackStartedMillis;
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
    state.animationFrame
  );
}

// TODO: try updating avatar based on pitch being played
void randomizeAvatar() {
  state.animationFrame = random(0, AVATAR_FRAMES - 1);
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
    String(state.trackIndex + 1) + "/" + String(SONGS_COUNT)
  );

  tinyfont.setCursor(x, y + CHAR_SIZE + GAP_MAX);
  tinyfont.print(readFlashStringPointer(&SONG_TITLES[TRACKS[state.trackIndex]]));
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
    SONG_LENGTHS[TRACKS[state.trackIndex]]
  );

  arduboy.drawRect(
    x + TIME_WIDTH + GAP_MIN, y,
    rectWidth,
    PROGRESS_BAR_HEIGHT
  );
  arduboy.fillRect(
    x + TIME_WIDTH + GAP_MIN, y,
    rectWidth * float(getElapsedPlayTime()) / SONG_LENGTHS[TRACKS[state.trackIndex]],
    PROGRESS_BAR_HEIGHT
  );
}

void drawIntro() {
  arduboy.fillRect(0, 0, WIDTH, HEIGHT);

  if (state.animationFrame <= INTRO_FRAMES) {
    SpritesB::drawOverwrite(
      WIDTH - 92,
      0,
      walk,
      state.animationFrame
    );
  }

  tinyfont.setTextColor(BLACK);
  tinyfont.setCursor(5, 5);
  tinyfont.print("SONGS\nFOR\nDESMOND");

  // TODO: remove
  tinyfont.setCursor(115, 5);
  tinyfont.print(state.animationFrame);
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
  if (state.trackIndex == newTrackIndex) {
    return;
  }

  state.trackIndex = newTrackIndex;

  if (state.isPlaying) {
    playCurrentSong();
  }
}

void handleOperationButtonPresses() {
  if (arduboy.justPressed(A_BUTTON)) {
    state.isPlaying = false;
  } else if (arduboy.justPressed(B_BUTTON)) {
    state.isPlaying = true;
    playCurrentSong();
  }

  if (arduboy.justPressed(RIGHT_BUTTON)) {
    changeTrack(state.trackIndex + 1);
    randomizeAvatar();
  } else if (arduboy.justPressed(LEFT_BUTTON)) {
    changeTrack(state.trackIndex - 1);
    randomizeAvatar();
  }

  if (state.trackIndex < 0 || state.trackIndex >= SONGS_COUNT) {
    reset();
  }
}

void handleIntroButtonPresses() {
  if (
    arduboy.justPressed(A_BUTTON) ||
    arduboy.justPressed(B_BUTTON) ||
    arduboy.justPressed(RIGHT_BUTTON) ||
    state.animationFrame > FPS * INTRO_SECONDS
  ) {
    state.animationFrame = 0;
    state.stage = Operation;
    tinyfont.setTextColor(WHITE);
  } else if (arduboy.justPressed(LEFT_BUTTON)) {
    state.animationFrame = 0;
    state.stage = Operation;
    tinyfont.setTextColor(WHITE);
    changeTrack(SONGS_COUNT - 1);
  }
}

void playCurrentSong() {
  arduboyTones.tones(SONG_SCORES[TRACKS[state.trackIndex]]);
  state.trackStartedMillis = millis();
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();

  if (
    state.isPlaying &&
    getElapsedPlayTime() >= SONG_LENGTHS[TRACKS[state.trackIndex]]
  ) {
    if (state.trackIndex < SONGS_COUNT - 1) {
      changeTrack(state.trackIndex + 1);
    } else if (state.trackIndex >= SONGS_COUNT - 1) {
      reset();
    }
  }

  if (!state.isPlaying) {
    arduboyTones.noTone();
  }

  arduboy.clear();

  if (state.stage == Intro) {
    if (arduboy.everyXFrames(INTRO_FRAMERATE)) {
      state.animationFrame++;
    }

    drawIntro();
    handleIntroButtonPresses();
  } else {
    if (state.isPlaying) {
      if (arduboy.everyXFrames(AVATAR_FRAMERATE)) {
        randomizeAvatar();
      }
    }

    drawOperation();
    handleOperationButtonPresses();
  }

  arduboy.display();
}
