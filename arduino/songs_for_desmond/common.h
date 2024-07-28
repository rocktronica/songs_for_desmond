#ifndef Common_h
#define Common_h

#include "song_lengths.h"
#include "song_scores.h"
#include "song_titles.h"

#include "FlashStringHelper.h"

// Songs are songs; tracks are ordered songs
const int8_t TRACKS[] = {
  WHOS_MY_LITTLE_BABY_BIRD,
  ANY_KIND_OF_CAR_AND_ANY_KIND_OF_TRUCK,
  WERE_ALMOST_HOME,
  MY_BABY_LOVES_TO_GO_POOPY,
  WASHING_YOUR_FACE,
  WHOS_MY_LITTLE_DAIKINI_BABY,
  THIS_TOWNS_GOT_A_LOT_OF_CONSTRUCTION,
};

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