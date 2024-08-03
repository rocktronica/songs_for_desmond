#pragma once

#include "song_lengths.h"
#include "song_scores.h"
#include "song_titles.h"
#include "tracks.h"
#include "FlashStringHelper.h"

# define TRACK_GAP    500

enum Stage {
  Intro,
  Operation
};

struct State {
  Stage stage;
  int8_t animationFrame;

  int8_t trackIndex;
  uint16_t trackStartedMillis;

  bool isPlaying;
};

int16_t getArduboyTonesBeat(uint16_t millis, uint16_t bpm) {
  // HACK: adjust timing for ArduboyTones 1024ms long second
  return floor(millis * 1.024 / (60000 / bpm));
}

uint16_t getSongLength(int8_t trackIndex) {
  return SONG_LENGTHS[TRACKS[trackIndex]] + TRACK_GAP;
}

uint16_t getSongBPM(int8_t trackIndex) {
  return SONG_BPMS[TRACKS[trackIndex]];
}

uint16_t getSongScore(int8_t trackIndex) {
  return SONG_SCORES[TRACKS[trackIndex]];
}

// NOTE: Ideally, this'd be getSongTitle and display code would stay in
// display.h where it belongs, but passing progmem references is beyond
// my understanding, so this is what I gotta do to move on with my life
void printSongTitle(int8_t trackIndex, Tinyfont& tinyfont) {
  tinyfont.print(readFlashStringPointer(&SONG_TITLES[TRACKS[trackIndex]]));
}

uint16_t getElapsedPlayTime(State& state) {
  if (state.isPlaying) {
    return millis() - state.trackStartedMillis;
  }

  return 0;
}