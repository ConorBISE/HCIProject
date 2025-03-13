#ifndef SCREEN_H
#define SCREEN_H

#include <Adafruit_ILI9341.h>

class Screen {
public:
    Screen(Adafruit_ILI9341& display) : display(display) {}
    virtual ~Screen() {}

    virtual void update() = 0;

protected:
Adafruit_ILI9341& display;
};

#endif // SCREEN_H