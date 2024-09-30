#pragma once

#include "common.h"

inline int16_t getArduboyTonesBeat(uint16_t millis, uint16_t bpm) {
  // HACK: adjust timing for ArduboyTones 1024ms long second
  // TODO: investigate effects of (60000 / bpm) being int instead of float
  return (static_cast<uint32_t>(millis) * 1024 / (60000 / bpm)) / 1000;
}

inline uint8_t getSongIndex(uint8_t trackIndex) {
  return pgm_read_byte(&tracks[trackIndex]);
}

inline uint16_t getSongLength(int8_t trackIndex) {
  return songLengths[getSongIndex(trackIndex)] + TRACK_GAP;
}

inline uint16_t getSongBPM(int8_t trackIndex) {
  return songBpms[getSongIndex(trackIndex)];
}

inline FlashStringHelper getPrintableSongTitle(int8_t trackIndex) {
  return readFlashStringPointer(&songTitles[getSongIndex(trackIndex)]);
}

inline const uint16_t *getSongScore(int8_t trackIndex) {
  return songScores[getSongIndex(trackIndex)];
}

inline uint16_t getElapsedPlayTime(State &state) {
  if (state.isPlaying) {
    return millis() - state.eventStarted;
  }

  return 0;
}