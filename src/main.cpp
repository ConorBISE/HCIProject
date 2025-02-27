#include <Arduino.h>
#include "integrations/ancs/ancs.h" 
#include <WiFi.h>

#define WIFI_SSID "dev"
#define WIFI_PWD "testtest"

static char LOG_TAG[] = "Main";

void setup()
{
    Serial.begin(115200);

    ESP_LOGI(LOG_TAG, "Connecting to wifi");

    WiFi.begin(WIFI_SSID, WIFI_PWD);

    while (WiFi.status() != WL_CONNECTED) {
        ESP_LOGI(LOG_TAG, ".");
        delay(1000);
    }

    ESP_LOGI(LOG_TAG, "\nConnected to wifi!");
}

void loop()
{

}
