#ifndef POMODORO_SCREEN_H
#define POMODORO_SCREEN_H

#include "screen.h"

enum class PomodoroState {
    WaitingForWorkConfirmation,
    WorkTimer,
    WaitingForBreakConfirmation,
    BreakTimer
};


class PomodoroScreen : public Screen {
public:
    PomodoroScreen(Adafruit_ILI9341* tft, CRGB* leds) : Screen(tft, leds) {}
    ~PomodoroScreen() {}

    void onEnter() override;
    void update(ButtonEvent* event) override;

private:
    void printCenteredTimeRemaining(int remainingTime);
    
    PomodoroState state = PomodoroState::WaitingForWorkConfirmation;
    time_t timerStart;
    time_t timerLength;
};

#endif // POMODORO_SCREEN_H