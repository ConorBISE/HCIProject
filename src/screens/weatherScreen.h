#ifndef WEATHERSCREEN_H
#define WEATHERSCREEN_H

#include "screen.h"

class WeatherScreen : public Screen {
public:
    WeatherScreen(Adafruit_ILI9341* tft, CRGB* leds) : Screen(tft, leds) {}
    ~WeatherScreen() {}

    void onEnter() override;
    void update(ButtonEvent* event) override;

private:
    void drawWeather();
    time_t nextUpdateTime;
};

#endif // WEATHERSCREEN_H