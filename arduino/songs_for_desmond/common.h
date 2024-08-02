#ifndef Common_h
#define Common_h

#include "song_lengths.h"
#include "song_scores.h"
#include "song_titles.h"

#include "tracks.h"

#include "FlashStringHelper.h"

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

uint16_t getSongLength(int8_t trackIndex) {
  return SONG_LENGTHS[TRACKS[trackIndex]];
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

#endif