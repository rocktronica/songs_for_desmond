#pragma once

#include "Display.h"

void Display::resetAnimation() { animationFrame = 0; }

void Display::incrementAnimation() { animationFrame++; }

void Display::randomizeAvatar() {
  int8_t newFrame = random(0, avatarFrames + 1);

  if (newFrame == animationFrame) {
    randomizeAvatar();
  } else {
    animationFrame = newFrame;
  }
}

void Display::drawAvatarFirst(int8_t x, int8_t y) {
  Sprites::drawOverwrite(x - avatarXOffset, y - avatarYOffset, walk,
                         animationFrame);
  Arduboy2Base::drawRect(x, y, avatarWidth, avatarHeight);

  // HACK: fill remaining space to cover overflow...
  // It's easier than masking, as long as it's done first!
  Arduboy2Base::fillRect(0, 0, WIDTH, y, BLACK);
  Arduboy2Base::fillRect(0, y, x, avatarHeight, BLACK);
  Arduboy2Base::fillRect(x + avatarWidth, y, WIDTH - x - avatarWidth,
                         avatarHeight, BLACK);
  Arduboy2Base::fillRect(0, y + avatarHeight, WIDTH, HEIGHT - y - avatarHeight,
                         BLACK);
}

void Display::drawPrettyTime(int8_t x, int8_t y,

                             uint16_t millis) {
  uint16_t seconds = millis / 1000;
  uint16_t minutes = seconds / 60;

  tinyfont.setCursor(x, y);
  tinyfont.print(minutes);

  tinyfont.setCursor(x + charSize, y);
  tinyfont.print(F(":"));

  tinyfont.setCursor(x + charSize + 3, y);
  tinyfont.print(seconds < 10 ? F("0") : F(""));
  tinyfont.print(seconds);
}

void Display::drawProgressBar(int8_t x, int8_t y,

                              uint8_t width,

                              State &state) {
  uint8_t rectWidth = width - (timeWidth + gapMin) * 2;

  drawPrettyTime(x, y, getElapsedPlayTime(state));
  drawPrettyTime(x + width - timeWidth, y, getSongLength(state.trackIndex));

  Arduboy2Base::drawRect(x + timeWidth + gapMin, y, rectWidth,
                         progressBarHeight);
  Arduboy2Base::fillRect(x + timeWidth + gapMin, y,
                         rectWidth * float(getElapsedPlayTime(state)) /
                             getSongLength(state.trackIndex),
                         progressBarHeight);
}

void Display::drawVolume(State &state) {
  Sprites::drawOverwrite(WIDTH - gapOuter - volumeSpriteWidth, operationTextY,
                         volume, state.volume);
}

void Display::drawIntro(State &state) {
  if (animationFrame <= introFrames) {
    Sprites::drawOverwrite(WIDTH - walkSpriteWidth, 0, walk, animationFrame);
  }

  tinyfont.setCursor(gapOuter, gapOuter);
  tinyfont.print(F("SONGS\nFOR\nDESMOND"));

  drawVolume(state);

  if (animationFrame > introFrames) {
    tinyfont.setCursor(WIDTH - charSize * 4 - 1 * (4 - 1) - gapOuter,
                       HEIGHT - charSize * 2 - 1 * (2 - 1) - gapOuter);
    tinyfont.print(F("2024\nDADA"));
  }
}

void Display::drawOperation(State &state, int8_t songsCount) {
  drawAvatarFirst(gapOuter, gapOuter);

  tinyfont.setCursor(operationTextX, operationTextY);
  tinyfont.print(state.trackIndex + 1);
  tinyfont.print(F("/"));
  tinyfont.print(songsCount);

  tinyfont.setCursor(operationTextX, operationTextY + charSize + gapMax);
  tinyfont.print(getPrintableSongTitle(state.trackIndex));

  drawVolume(state);

  drawProgressBar(gapOuter, HEIGHT - progressBarHeight - gapOuter,
                  WIDTH - gapOuter * 2, state);
}