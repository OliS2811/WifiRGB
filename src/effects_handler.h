#ifndef EFFECTS_HANDLER_H
#define EFFECTS_HANDLER_H

#include <Adafruit_NeoPixel.h>
#include "names.h"

extern Adafruit_NeoPixel np;

void partyMode() {
  for (int i = 0; i < NUMPIXELS; i++) {
    np.setPixelColor(i, np.Color(random(0, 256), random(0, 256), random(0, 256)));
  }
  np.show();
  delay(100);
}

void romanticMode() {
  for (int i = 0; i < NUMPIXELS; i++) {
    np.setPixelColor(i, np.Color(255, 69, 0));
  }
  np.show();
}

void readingMode() {
  for (int i = 0; i < NUMPIXELS; i++) {
    np.setPixelColor(i, np.Color(255, 255, 255));
  }
  np.show();
}

void nightMode() {
  for (int i = 0; i < NUMPIXELS; i++) {
    np.setPixelColor(i, np.Color(0, 0, 128));
  }
  np.show();
}

#endif
