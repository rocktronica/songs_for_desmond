#pragma once

#include <ArduboyTones.h>

#include "FlashStringHelper.h"
#include "song_bpms.h"
#include "song_lengths.h"
#include "song_scores.h"
#include "song_titles.h"
#include "tracks.h"

const int trackGap = 500;

const int8_t songsCount = sizeof(tracks) / sizeof(int8_t);

// NOTE: Relying on implicit conversion to integer.
// Orders are intentional!
enum Stage { Intro, Operation };
enum Volume { Mute, Low, High };

struct State {
  Stage stage;

  int8_t trackIndex;
  uint32_t eventStarted;

  Volume volume;

  bool isPlaying;
};