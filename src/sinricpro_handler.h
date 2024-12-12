#ifndef SINRICPRO_HANDLER_H
#define SINRICPRO_HANDLER_H

#include <SinricPro.h>
#include <SinricProLight.h>
#include "effects_handler.h"
#include "names.h"

extern String currentScene;
extern Adafruit_NeoPixel np;
extern int brightness;

void setupSinricPro(const char* appKey, const char* appSecret, const char* lightId) {
  SinricProLight& myLight = SinricPro[lightId];

  myLight.onPowerState([](const String &deviceId, bool &state) {
    if (state) {
      currentScene = "party";
    } else {
      currentScene = "";
      for (int i = 0; i < NUMPIXELS; i++) {
        np.setPixelColor(i, 0);
      }
      np.show();
    }
    return true;
  });

  myLight.onBrightness([](const String &deviceId, int &level) {
    brightness = map(level, 0, 100, 0, MAX_BRIGHTNESS);
    np.setBrightness(brightness);
    np.show();
    return true;
  });

  myLight.onColor([](const String &deviceId, uint8_t &r, uint8_t &g, uint8_t &b) {
    currentScene = "";
    for (int i = 0; i < NUMPIXELS; i++) {
      np.setPixelColor(i, np.Color(r, g, b));
    }
    np.show();
    return true;
  });

  SinricPro.begin(appKey, appSecret);
}

#endif
