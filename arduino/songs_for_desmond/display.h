#include "common.h"
#include "graphics.h"

// REMEMBER: screen size is 128x64

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

# define OPERATION_TEXT_X     GAP_MAX + AVATAR_WIDTH + GAP_MAX
# define OPERATION_TEXT_Y     GAP_MAX

void drawAvatar(
  int8_t x,
  int8_t y,

  int8_t animationFrame,

  Arduboy2& arduboy
) {
  arduboy.fillRect(x, y, AVATAR_WIDTH, AVATAR_HEIGHT);

  SpritesB::drawOverwrite(
    x + (AVATAR_WIDTH - 35) / 2,
    y + AVATAR_HEIGHT - 40 - 1,
    avatar,
    animationFrame
  );
}

void drawPrettyTime(
  int8_t x,
  int8_t y,

  uint16_t millis,

  Tinyfont& tinyfont
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

void drawProgressBar(
  int8_t x,
  int8_t y,

  uint8_t width,

  State& state,

  Arduboy2& arduboy,
  Tinyfont& tinyfont
) {
  uint8_t rectWidth = width - (TIME_WIDTH + GAP_MIN) * 2;

  drawPrettyTime(
    x, y,
    getElapsedPlayTime(state),
    tinyfont
  );
  drawPrettyTime(
    x + width - TIME_WIDTH, y,
    getSongLength(state.trackIndex),
    tinyfont
  );

  arduboy.drawRect(
    x + TIME_WIDTH + GAP_MIN, y,
    rectWidth,
    PROGRESS_BAR_HEIGHT
  );
  arduboy.fillRect(
    x + TIME_WIDTH + GAP_MIN, y,
    rectWidth * float(getElapsedPlayTime(state)) / getSongLength(state.trackIndex),
    PROGRESS_BAR_HEIGHT
  );
}

void drawIntro(
  State& state,

  Arduboy2& arduboy,
  Tinyfont& tinyfont
) {

  if (state.animationFrame <= INTRO_FRAMES) {
    SpritesB::drawOverwrite(
      WIDTH - 92,
      0,
      walk,
      state.animationFrame
    );
  }

  tinyfont.setCursor(GAP_MAX, GAP_MAX);
  tinyfont.print("SONGS\nFOR\nDESMOND");

  if (state.animationFrame > INTRO_FRAMES + 1) {
    tinyfont.setCursor(
      WIDTH - CHAR_SIZE * 4 - 1 * (4 - 1) - GAP_MAX,
      HEIGHT - CHAR_SIZE * 2 - 1 * (2 - 1) - GAP_MAX
    );
    tinyfont.print("2024\nDADA");
  }
}

void drawOperation(
  State& state,

  int8_t songsCount,

  Arduboy2& arduboy,
  Tinyfont& tinyfont
) {
  drawAvatar(GAP_MAX, GAP_MAX, state.animationFrame, arduboy);

  tinyfont.setCursor(OPERATION_TEXT_X, OPERATION_TEXT_Y);
  tinyfont.print(String(state.trackIndex + 1) + "/" + String(songsCount));
  tinyfont.setCursor(OPERATION_TEXT_X, OPERATION_TEXT_Y + CHAR_SIZE + GAP_MAX);
  printSongTitle(state.trackIndex, tinyfont);

  drawProgressBar(
    GAP_MAX, HEIGHT - PROGRESS_BAR_HEIGHT - GAP_MAX,
    WIDTH - GAP_MAX * 2,
    state,
    arduboy, tinyfont
  );
}