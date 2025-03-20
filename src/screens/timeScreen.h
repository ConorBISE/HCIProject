#ifndef TIMESCREEN_H
#define TIMESCREEN_H

#include "screen.h"

class TimeScreen : public Screen {
public:
    TimeScreen(Adafruit_ILI9341* tft, CRGB* leds) : Screen(tft, leds) {}
    ~TimeScreen() {}

    void onEnter() override;
    void update(ButtonEvent* event) override;

private:
    void drawTime();
    time_t nextUpdateTime;
};

#endif // TIMESCREEN_H