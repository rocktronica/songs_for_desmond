#pragma once

#include <Arduboy2.h>
#include <Tinyfont.h>

#include "common.h"
#include "graphics.h"
#include "utils.h"

// REMEMBER: screen size is 128x64

const int charSize = 4; // TinyFont 4x4

const int gapMin = 1;
const int gapMax = 4;
const int gapOuter = 4;

const int timeWidth = (charSize * 3 + 4);
const int progressBarHeight = charSize;

const int fps = 12;
const int introSeconds = 3;
const int introFrames = 10;
const int introFramerate = (fps / 6);

const int avatarHeight = (HEIGHT - progressBarHeight - gapOuter * 2 - gapMax);
const int avatarWidth = avatarHeight;
const int avatarFrames = introFrames - 2;
const int avatarXOffset = 8;
const int avatarYOffset = 4;
const int walkSpriteWidth = 92;

const int operationTextX = gapOuter + avatarWidth + gapMax;
const int operationTextY = gapOuter;

const int volumeSpriteWidth = 7;
const int volumeSpriteHeight = 4;

class Display {
private:
  int8_t animationFrame{0};
  Tinyfont tinyfont{Arduboy2Base::sBuffer, WIDTH, HEIGHT};

public:
  void resetAnimation();

  void incrementAnimation();

  void randomizeAvatar();

  void drawAvatarFirst(int8_t x, int8_t y);

  void drawPrettyTime(int8_t x, int8_t y, uint16_t millis);

  void drawProgressBar(int8_t x, int8_t y, uint8_t width, State &state);

  void drawVolume(State &state);

  void drawIntro(State &state);

  void drawOperation(State &state, int8_t songsCount);
};