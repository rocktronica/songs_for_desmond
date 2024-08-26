#pragma once

#include <Arduboy2.h>
#include <Tinyfont.h>

#include "common.h"
#include "utils.h"
#include "graphics.h"

// REMEMBER: screen size is 128x64

# define CHAR_SIZE            4    // TinyFont 4x4

# define GAP_MIN              1
# define GAP_MAX              4
# define GAP_OUTER            4

# define TIME_WIDTH           (CHAR_SIZE * 3 + 4)
# define PROGRESS_BAR_HEIGHT  CHAR_SIZE

# define FPS                  12
# define INTRO_SECONDS        3
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

class Display {
  private:
    int8_t animationFrame;
    Tinyfont* tinyfont;

  public:
    Display(Arduboy2& arduboy);

    void resetAnimation();

    void incrementAnimation();

    void randomizeAvatar();

    void drawAvatarFirst(
      int8_t x, int8_t y,
      Arduboy2& arduboy
    );

    void drawPrettyTime(
      int8_t x, int8_t y,
      uint16_t millis
    );

    void drawProgressBar(
      int8_t x, int8_t y,
      uint8_t width,
      State& state,
      Arduboy2& arduboy
    );

    void drawVolume(
      int8_t x, int8_t y,
      State& state
    );

    void drawIntro(
      State& state,
      Arduboy2& arduboy
    );

    void drawOperation(
      State& state,
      int8_t songsCount,
      Arduboy2& arduboy
    );
};