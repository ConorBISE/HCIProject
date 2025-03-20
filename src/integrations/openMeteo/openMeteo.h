#ifndef OPEN_METEO_H
#define OPEN_METEO_H

#include <Arduino.h>

struct WeatherData {
    float temperature;
    float humidity;
    int weatherCode;
};

static String WEATHER_CODE_MAP[] = {
    "Sunny", "Mainly Sunny", "Partly Cloudy", "Cloudy",
    "Foggy","Rime Fog","Light Drizzle","Drizzle",
    "Heavy Drizzle","Light Freezing Drizzle","Freezing Drizzle","Light Rain",
    "Rain","Heavy Rain","Light Freezing Rain","Freezing Rain",
    "Light Snow","Snow","Heavy Snow","Snow Grains",
    "Light Showers","Showers","Heavy Showers","Light Snow Showers",
    "Snow Showers","Thunderstorm","Light Thunderstorms With Hail","Thunderstorm With Hail"
};

class OpenMeteoAPI {
public:
    OpenMeteoAPI() {};
    WeatherData getWeather();
};

#endif // OPEN_METEO_H
