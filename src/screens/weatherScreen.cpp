#include "weatherScreen.h"

#include "../integrations/openMeteo/openMeteo.h"

void WeatherScreen::onEnter() {
    tft->fillScreen(ILI9341_BLACK);
    drawWeather();
}

void WeatherScreen::update(ButtonEvent* event) {
    tm now;

    if (getLocalTime(&now)) {
        time_t now_int = mktime(&now);
    
        if (now_int > nextUpdateTime) {
            drawWeather();
        
            now.tm_sec = 0;
            now.tm_min += 1;
            nextUpdateTime = mktime(&now);    
        }
    }  
}

void WeatherScreen::drawWeather() {
    OpenMeteoAPI weatherClient;
    WeatherData weatherData = weatherClient.getWeather();

    String weatherCondition = WEATHER_CODE_MAP[weatherData.weatherCode];

    tft->fillScreen(ILI9341_BLACK);
    tft->setTextSize(2);

    // Calculate and draw weather condition text
    int16_t x1, y1;
    uint16_t w, h;
    tft->getTextBounds(weatherCondition, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (tft->width() - w) / 2;
    int16_t y = (tft->height() / 2) - 20; // Adjusted for spacing
    tft->setCursor(x, y);
    tft->print(weatherCondition);

    // Prepare location, temperature, and humidity string
    char detailsStr[32];
    snprintf(detailsStr, sizeof(detailsStr), "Limerick | %.0fC | %.0f%%", weatherData.temperature, weatherData.humidity);

    // Calculate and draw details text
    tft->getTextBounds(detailsStr, 0, 0, &x1, &y1, &w, &h);

    x = (tft->width() - w) / 2;
    y += 30; // Spacing below weather condition

    tft->setCursor(x, y);
    tft->print(detailsStr);
}