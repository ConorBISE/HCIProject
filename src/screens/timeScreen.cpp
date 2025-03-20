#include "timeScreen.h"

void TimeScreen::onEnter() {
    drawTime();
}

void TimeScreen::update(ButtonEvent* event) {
    tm now;
    if (getLocalTime(&now)) {
        time_t now_int = mktime(&now);
        if (now_int > nextUpdateTime) {
            drawTime();
        }
    }  
}

#define TIME_SIZE 6
#define DATE_SIZE 3

void TimeScreen::drawTime() {
    tft->fillScreen(ILI9341_BLACK);

    tm now;

    if (getLocalTime(&now)) {
        char timeStr[6]; // HH:MM
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d", now.tm_hour, now.tm_min);
        tft->setTextSize(TIME_SIZE);
        int16_t x1, y1, x2, y2;
        uint16_t w1, h1, w2, h2;

        // Get bounds for time
        tft->getTextBounds(timeStr, 0, 0, &x1, &y1, &w1, &h1);

        char dateStr[11]; // DD/MM/YYYY
        snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", now.tm_mday, now.tm_mon + 1, now.tm_year + 1900);
        tft->setTextSize(DATE_SIZE);

        // Get bounds for date
        tft->getTextBounds(dateStr, 0, 0, &x2, &y2, &w2, &h2);

        // Draw time
        tft->setTextSize(TIME_SIZE);
        tft->setCursor((tft->width() - w1) / 2, tft->height() / 2 - 20);
        tft->print(timeStr);

        // Draw date
        tft->setTextSize(DATE_SIZE);
        tft->setCursor((tft->width() - w2) / 2, tft->height() / 2 + 30);
        tft->print(dateStr);
        
        now.tm_sec = 0;
        now.tm_min += 1;
        nextUpdateTime = mktime(&now);    
    } else {
        Serial.println("Failed to obtain time");
    }
}