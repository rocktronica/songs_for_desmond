#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

#include "common.h"
#include "display.h"

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

State state = {
  Intro,
  0,

  0,
  0,

  false
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

// TODO: try updating avatar based on pitch being played
void updateAvatar() {
  state.animationFrame = map(
    getElapsedPlayTime(state),
    0, getSongLength(state.trackIndex),
    0, AVATAR_FRAMES + 1
  );
}

void changeTrack(int8_t newTrackIndex) {
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
    updateAvatar();
  } else if (arduboy.justPressed(LEFT_BUTTON)) {
    changeTrack(
      getElapsedPlayTime(state) < 1000
        ? state.trackIndex - 1
        : state.trackIndex
      );
    updateAvatar();
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
  arduboyTones.tones(getSongScore(state.trackIndex));
  state.trackStartedMillis = millis();
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();

  if (
    state.isPlaying &&
    getElapsedPlayTime(state) >= getSongLength(state.trackIndex)
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

    drawIntro(state, arduboy, tinyfont);
    handleIntroButtonPresses();
  } else {
    if (state.isPlaying) {
      updateAvatar();
    }

    drawOperation(
      state,
      SONGS_COUNT,
      arduboy, tinyfont
    );
    handleOperationButtonPresses();
  }

  arduboy.display();
}
