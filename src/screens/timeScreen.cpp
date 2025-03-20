#include "timeScreen.h"

void TimeScreen::onEnter() {
    tft->fillScreen(ILI9341_BLACK);
    drawWeather();
}

void TimeScreen::update(ButtonEvent* event) {
    tm now;
    if (getLocalTime(&now)) {
        time_t now_int = mktime(&now);
        if (now_int > nextUpdateTime) {
            drawWeather();
        }
    }  
}

void TimeScreen::drawWeather() {
    tm now;

    if (getLocalTime(&now)) {
        char timeStr[6]; // HH:MM
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d", now.tm_hour, now.tm_min);
        tft->setTextSize(3);
        int16_t x1, y1;
        uint16_t w, h;

        tft->getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
        tft->setCursor((tft->width() - w) / 2, tft->height() / 2 - 20);
        tft->print(timeStr);

        char dateStr[11]; // DD/MM/YYYY
        snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", now.tm_mday, now.tm_mon + 1, now.tm_year + 1900);
        tft->setTextSize(2);
        tft->getTextBounds(dateStr, 0, 0, &x1, &y1, &w, &h);
        tft->setCursor((tft->width() - w) / 2, tft->height() / 2 + 20);
        tft->print(dateStr);
        
        now.tm_sec = 0;
        now.tm_min += 1;
        nextUpdateTime = mktime(&now);    
    } else {
        Serial.println("Failed to obtain time");
    }
}