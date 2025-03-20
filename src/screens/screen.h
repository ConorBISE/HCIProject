#ifndef SCREEN_H
#define SCREEN_H

#include "screenEvent.h"

#include <Adafruit_ILI9341.h>

class CRGB;


class Screen {
public:
    Screen(Adafruit_ILI9341* tft, CRGB* leds) : tft(tft), leds(leds) {}
    virtual ~Screen() {}

    virtual void onEnter() = 0;
    virtual void update(ButtonEvent* event) = 0;

    void showNotification(String title, String subtitle, CRGB color);

protected:
    Adafruit_ILI9341* tft;
    CRGB* leds;
};

#endif // SCREEN_H