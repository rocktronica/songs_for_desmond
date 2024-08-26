#pragma once

#include "common.h"

inline int16_t getArduboyTonesBeat(uint16_t millis, uint16_t bpm) {
  // HACK: adjust timing for ArduboyTones 1024ms long second
  return floor(millis * 1.024 / (60000 / bpm));
}

inline uint16_t getSongLength(int8_t trackIndex) {
  return SONG_LENGTHS[TRACKS[trackIndex]] + TRACK_GAP;
}

inline uint16_t getSongBPM(int8_t trackIndex) {
  return SONG_BPMS[TRACKS[trackIndex]];
}

inline uint16_t getSongScore(int8_t trackIndex) {
  return SONG_SCORES[TRACKS[trackIndex]];
}

inline uint16_t getElapsedPlayTime(State& state) {
  if (state.isPlaying) {
    return millis() - state.eventStarted;
  }

  return 0;
}