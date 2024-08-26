#pragma once

#include <ArduboyTones.h>

#include "song_bpms.h"
#include "song_lengths.h"
#include "song_scores.h"
#include "song_titles.h"
#include "tracks.h"
#include "FlashStringHelper.h"

# define TRACK_GAP    500

const int8_t SONGS_COUNT = sizeof(TRACKS) / sizeof(int8_t);

enum Stage {
  Intro,
  Operation
};

enum Volume {
  Mute,
  Low,
  High
};

struct State {
  Stage stage;

  int8_t trackIndex;
  uint16_t eventStarted;

  Volume volume;

  bool isPlaying;
};