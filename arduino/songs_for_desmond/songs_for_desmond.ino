#include <Arduboy2.h>
#include <ArduboyTones.h>

#include "common.h"
#include "Display.h"

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);
Display display(arduboy);

State state = {
  Intro,
  0, 0,
  Low,
  false
};

void setStage(Stage stage) {
  state.stage = stage;
  state.eventStarted = millis();
  state.isPlaying = false;

  display.resetAnimation();

  changeTrack(0);
}

void reset() {
  setStage(Stage::Intro);
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FPS);
  reset();
}

bool hasBeenUpdatedThisBeat = false;
void updateAvatar() {
  int16_t doubleTempoBeat = getArduboyTonesBeat(
    getElapsedPlayTime(state),
    getSongBPM(state.trackIndex) * 2
  );

  if (doubleTempoBeat % 2 == 0) {
    if (!state.isPlaying || !hasBeenUpdatedThisBeat) {
      display.randomizeAvatar();
      hasBeenUpdatedThisBeat = true;
    }
  } else {
    hasBeenUpdatedThisBeat = false;
  }
}

void changeTrack(int8_t newTrackIndex) {
  state.trackIndex = newTrackIndex;

  if (state.isPlaying) {
    playCurrentSong();
  }
}

void updateVolume(Volume volume) {
  state.volume = constrain(volume, LOW, High);

  if (state.volume == Volume::Mute) {
    arduboy.audio.off();
  } else {
    arduboy.audio.on();
  }

  arduboyTones.volumeMode(
    state.volume == Volume::High
      ? VOLUME_ALWAYS_HIGH
      : VOLUME_ALWAYS_NORMAL
  );
}

void handleVolumeButtonPresses() {
  if (arduboy.justPressed(UP_BUTTON)) {
    updateVolume(state.volume + 1);
  } else if (arduboy.justPressed(DOWN_BUTTON)) {
    updateVolume(state.volume - 1);
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
    display.randomizeAvatar();
  } else if (arduboy.justPressed(LEFT_BUTTON)) {
    changeTrack(
      getElapsedPlayTime(state) < 1000
        ? state.trackIndex - 1
        : state.trackIndex
      );
    display.randomizeAvatar();
  }

  handleVolumeButtonPresses();

  if (state.trackIndex < 0 || state.trackIndex >= SONGS_COUNT) {
    reset();
  }
}

void handleIntroButtonPresses() {
  if (
    arduboy.justPressed(A_BUTTON) ||
    arduboy.justPressed(B_BUTTON) ||
    arduboy.justPressed(RIGHT_BUTTON)
  ) {
    setStage(Stage::Operation);
  } else if (arduboy.justPressed(LEFT_BUTTON)) {
    setStage(Stage::Operation);
    changeTrack(SONGS_COUNT - 1);
  }

  handleVolumeButtonPresses();
}

void playCurrentSong() {
  arduboyTones.tones(getSongScore(state.trackIndex));
  state.eventStarted = millis();
}

void loop() {
  if (!state.isPlaying) {
    arduboyTones.noTone();
  }

  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  if (state.stage == Intro) {
    if (arduboy.everyXFrames(INTRO_FRAMERATE)) {
      display.incrementAnimation();
    }

    display.drawIntro(state, arduboy);
    handleIntroButtonPresses();
  } else {
    uint16_t elapsedPlayTime = getElapsedPlayTime(state);
    uint16_t songLength = getSongLength(state.trackIndex);

    if (state.isPlaying && elapsedPlayTime >= songLength) {
      if (state.trackIndex < SONGS_COUNT - 1) {
        changeTrack(state.trackIndex + 1);
      } else if (state.trackIndex >= SONGS_COUNT - 1) {
        return reset();
      }
    }

    if (state.isPlaying && elapsedPlayTime < songLength - TRACK_GAP) {
      updateAvatar();
    }

    display.drawOperation(state, SONGS_COUNT, arduboy);
    handleOperationButtonPresses();
  }

  arduboy.display();

  if (
    state.stage == Intro &&
    (millis() - state.eventStarted) >= INTRO_SECONDS * 1000
  ) {
    setStage(Stage::Operation);
  }
}
