#pragma once

#include "Display.h"

Display::Display() {
  // TODO: avoid new, try member initializer list
  tinyfont = new Tinyfont(Arduboy2Base::sBuffer, WIDTH, HEIGHT);
}

void Display::resetAnimation() {
  animationFrame = 0;
}

void Display::incrementAnimation() {
  animationFrame++;
}

void Display::randomizeAvatar() {
  int8_t newFrame = random(0, AVATAR_FRAMES + 1);

  if (newFrame == animationFrame) {
    randomizeAvatar();
  } else {
    animationFrame = newFrame;
  }
}

void Display::drawAvatarFirst(
  int8_t x,
  int8_t y
) {
  SpritesB::drawOverwrite(
    x - AVATAR_X_OFFSET, y - AVATAR_Y_OFFSET,
    walk, animationFrame
  );
  Arduboy2Base::drawRect(x, y, AVATAR_WIDTH, AVATAR_HEIGHT);

  // HACK: fill remaining space to cover overflow...
  // It's easier than masking, as long as it's done first!
  Arduboy2Base::fillRect(0, 0, WIDTH, y, BLACK);
  Arduboy2Base::fillRect(0, y, x, AVATAR_HEIGHT, BLACK);
  Arduboy2Base::fillRect(
    x + AVATAR_WIDTH, y, WIDTH - x - AVATAR_WIDTH,
    AVATAR_HEIGHT,
    BLACK
  );
  Arduboy2Base::fillRect(
    0, y + AVATAR_HEIGHT,
    WIDTH, HEIGHT - y - AVATAR_HEIGHT,
    BLACK
  );
}

void Display::drawPrettyTime(
  int8_t x,
  int8_t y,

  uint16_t millis
) {
  uint16_t seconds = millis / 1000;
  uint16_t minutes = floor(seconds / 60);

  tinyfont->setCursor(x, y);
  tinyfont->print(minutes);

  tinyfont->setCursor(x + CHAR_SIZE, y);
  tinyfont->print(F(":"));

  tinyfont->setCursor(x + CHAR_SIZE + 3, y);
  tinyfont->print(seconds < 10 ? F("0") : F(""));
  tinyfont->print(seconds);
}

void Display::drawProgressBar(
  int8_t x,
  int8_t y,

  uint8_t width,

  State& state
) {
  uint8_t rectWidth = width - (TIME_WIDTH + GAP_MIN) * 2;

  drawPrettyTime(
    x, y,
    getElapsedPlayTime(state)
  );
  drawPrettyTime(
    x + width - TIME_WIDTH, y,
    getSongLength(state.trackIndex)
  );

  Arduboy2Base::drawRect(
    x + TIME_WIDTH + GAP_MIN, y,
    rectWidth,
    PROGRESS_BAR_HEIGHT
  );
  Arduboy2Base::fillRect(
    x + TIME_WIDTH + GAP_MIN, y,
    rectWidth * float(getElapsedPlayTime(state))
      / getSongLength(state.trackIndex),
    PROGRESS_BAR_HEIGHT
  );
}

void Display::drawVolume(State& state) {
  SpritesB::drawOverwrite(
    WIDTH - GAP_OUTER - VOLUME_SPRITE_WIDTH, OPERATION_TEXT_Y,
    volume, state.volume
  );
}

void Display::drawIntro(
  State& state
) {
  if (animationFrame <= INTRO_FRAMES) {
    SpritesB::drawOverwrite(
      WIDTH - WALK_SPRITE_WIDTH,
      0,
      walk,
      animationFrame
    );
  }

  tinyfont->setCursor(GAP_OUTER, GAP_OUTER);
  tinyfont->print(F("SONGS\nFOR\nDESMOND"));

  drawVolume(state);

  if (animationFrame > INTRO_FRAMES) {
    tinyfont->setCursor(
      WIDTH - CHAR_SIZE * 4 - 1 * (4 - 1) - GAP_OUTER,
      HEIGHT - CHAR_SIZE * 2 - 1 * (2 - 1) - GAP_OUTER
    );
    tinyfont->print(F("2024\nDADA"));
  }
}

void Display::drawOperation(
  State& state,
  int8_t songsCount
) {
  drawAvatarFirst(GAP_OUTER, GAP_OUTER);

  tinyfont->setCursor(OPERATION_TEXT_X, OPERATION_TEXT_Y);
  tinyfont->print(state.trackIndex + 1);
  tinyfont->print(F("/"));
  tinyfont->print(songsCount);

  tinyfont->setCursor(
    OPERATION_TEXT_X,
    OPERATION_TEXT_Y + CHAR_SIZE + GAP_MAX
  );
  // NOTE: Ideally wouldn't have any SONG_ stuff here and would
  // rely solely on getSong...() utils, but passing progmem
  // references is beyond my understanding, so this is what I
  // gotta do to move on with my life
  tinyfont->print(
    readFlashStringPointer(&SONG_TITLES[TRACKS[state.trackIndex]])
  );

  drawVolume(state);

  drawProgressBar(
    GAP_OUTER, HEIGHT - PROGRESS_BAR_HEIGHT - GAP_OUTER,
    WIDTH - GAP_OUTER * 2,
    state
  );
}