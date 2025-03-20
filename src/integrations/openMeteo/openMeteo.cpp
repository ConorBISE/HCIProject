#include "openMeteo.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

static char LOG_TAG[] = "OpenMeteo";

WeatherData OpenMeteoAPI::getWeather() {
    HTTPClient client;

    String url = "http://192.168.137.1:8000/weather";
    client.begin(url);

    int res = client.GET();
    if (res > 0) {
        auto out = client.getString();
        JsonDocument doc;
        deserializeJson(doc, out);

        JsonObject currentWeather = doc["current"];

        WeatherData data;
        data.temperature = currentWeather["temperature_2m"].as<float>();
        data.humidity = currentWeather["relative_humidity_2m"].as<float>();
        data.weatherCode = currentWeather["weather_code"].as<int>();
        return data;

    } else {
        ESP_LOGE(LOG_TAG, "Error fetching weather data from OpenMeteo API!");
    }

    return {0.0, 0.0, 0};
}
