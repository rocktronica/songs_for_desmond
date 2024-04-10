#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "songs.h"

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  if (arduboy.justPressed(B_BUTTON)) {
    arduboyTones.tones(WASHING_YOUR_FACE);
  }

  arduboy.display();
}
