#include "pomodoroScreen.h"

// #define WORK_LENGTH 25 * 60
// #define BREAK_LENGH 5 * 60
#define WORK_LENGTH 25
#define BREAK_LENGTH 5

time_t getNow() {
    tm now;

    if (getLocalTime(&now)) {
        return mktime(&now);
    }
}

void PomodoroScreen::printCenteredTimeRemaining(int remainingTime) {
    int minutes = remainingTime / 60;
    int seconds = (remainingTime % 60);

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);

    int16_t x1, y1;
    uint16_t w, h;
    
    tft->setTextSize(4);
    tft->getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (tft->width() - w) / 2;
    int16_t y = (tft->height() - h) / 2;

    tft->fillRect(0, 0, tft->width(), tft->height(), ILI9341_BLACK); // Clear screen
    tft->setCursor(x, y);
    tft->setTextColor(ILI9341_WHITE);
    tft->print(buffer);
}

void PomodoroScreen::onEnter() {
    tft->fillScreen(ILI9341_BLACK);

    switch (state) {
        case PomodoroState::WaitingForWorkConfirmation:
            printCenteredTimeRemaining(WORK_LENGTH);
            break;
        
        case PomodoroState::WaitingForBreakConfirmation:
            printCenteredTimeRemaining(BREAK_LENGTH);
            break;
    }
}

void PomodoroScreen::update(ButtonEvent* event) {
    switch (state) {
        case PomodoroState::WaitingForWorkConfirmation:
            if (event && event->position == ButtonPosition::Top && event->state == ButtonState::Pressed) {
                state = PomodoroState::WorkTimer;
                timerLength = WORK_LENGTH;
                timerStart = getNow();
            }
        
            break;

        case PomodoroState::WorkTimer:
            {
                time_t now = getNow();
                int remainingTime = timerLength - (now - timerStart);

                if (remainingTime <= 0) {
                    printCenteredTimeRemaining(BREAK_LENGTH);
                    state = PomodoroState::WaitingForBreakConfirmation;
                } else {
                    printCenteredTimeRemaining(remainingTime);
                }
            }
            break;

        case PomodoroState::WaitingForBreakConfirmation:
            if (event && event->position == ButtonPosition::Top && event->state == ButtonState::Pressed) {
                state = PomodoroState::BreakTimer;
                timerLength = BREAK_LENGTH;
                timerStart = getNow();
            }
            break;

        case PomodoroState::BreakTimer:
            {
                time_t now = getNow();
                int remainingTime = timerLength - (now - timerStart);

                if (remainingTime <= 0) {
                    printCenteredTimeRemaining(WORK_LENGTH);
                    state = PomodoroState::WaitingForWorkConfirmation;
                } else {
                    printCenteredTimeRemaining(remainingTime);
                }
            }
        break;
    }
}
