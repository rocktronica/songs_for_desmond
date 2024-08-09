#include "common.h"
#include "graphics.h"

// REMEMBER: screen size is 128x64

# define CHAR_SIZE            4    // TinyFont 4x4

# define GAP_MIN              1
# define GAP_MAX              4
# define GAP_OUTER            4

# define TIME_WIDTH           (CHAR_SIZE * 3 + 4)
# define PROGRESS_BAR_HEIGHT  CHAR_SIZE

# define FPS                  12
# define INTRO_SECONDS        1.5
# define INTRO_FRAMES         10
# define INTRO_FRAMERATE      (FPS / 6)

# define AVATAR_HEIGHT        (HEIGHT - PROGRESS_BAR_HEIGHT - GAP_OUTER * 2 - GAP_MAX)
# define AVATAR_WIDTH         AVATAR_HEIGHT
# define AVATAR_FRAMES        INTRO_FRAMES - 2
# define AVATAR_X_OFFSET      8
# define AVATAR_Y_OFFSET      4

# define OPERATION_TEXT_X     GAP_OUTER + AVATAR_WIDTH + GAP_MAX
# define OPERATION_TEXT_Y     GAP_OUTER

# define VOLUME_SPRITE_WIDTH  7
# define VOLUME_SPRITE_HEIGHT 4

void drawAvatarFirst(
  int8_t x,
  int8_t y,

  int8_t animationFrame,

  Arduboy2& arduboy
) {
  SpritesB::drawOverwrite(
    x - AVATAR_X_OFFSET, y - AVATAR_Y_OFFSET,
    walk, animationFrame
  );
  arduboy.drawRect(x, y, AVATAR_WIDTH, AVATAR_HEIGHT);

  // HACK: fill remaining space to cover overflow...
  // It's easier than masking, as long as it's done first!
  arduboy.fillRect(0, 0, WIDTH, y, BLACK);
  arduboy.fillRect(0, y, x, AVATAR_HEIGHT, BLACK);
  arduboy.fillRect(x + AVATAR_WIDTH, y, WIDTH - x - AVATAR_WIDTH, AVATAR_HEIGHT, BLACK);
  arduboy.fillRect(0, y + AVATAR_HEIGHT, WIDTH, HEIGHT - y - AVATAR_HEIGHT, BLACK);
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

void drawVolume(
  int8_t x,
  int8_t y,

  State& state,

  Arduboy2& arduboy
) {
  SpritesB::drawOverwrite(
    x, y,
    volume, state.volume
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

  tinyfont.setCursor(GAP_OUTER, GAP_OUTER);
  tinyfont.print("SONGS\nFOR\nDESMOND");

  if (state.animationFrame > INTRO_FRAMES + 1) {
    tinyfont.setCursor(
      WIDTH - CHAR_SIZE * 4 - 1 * (4 - 1) - GAP_OUTER,
      HEIGHT - CHAR_SIZE * 2 - 1 * (2 - 1) - GAP_OUTER
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
  drawAvatarFirst(GAP_OUTER, GAP_OUTER, state.animationFrame, arduboy);

  tinyfont.setCursor(OPERATION_TEXT_X, OPERATION_TEXT_Y);
  tinyfont.print(String(state.trackIndex + 1) + "/" + String(songsCount));
  tinyfont.setCursor(OPERATION_TEXT_X, OPERATION_TEXT_Y + CHAR_SIZE + GAP_MAX);
  printSongTitle(state.trackIndex, tinyfont);

  drawVolume(
    WIDTH - GAP_OUTER - VOLUME_SPRITE_WIDTH, OPERATION_TEXT_Y,
    state,
    arduboy
  );

  drawProgressBar(
    GAP_OUTER, HEIGHT - PROGRESS_BAR_HEIGHT - GAP_OUTER,
    WIDTH - GAP_OUTER * 2,
    state,
    arduboy, tinyfont
  );
}