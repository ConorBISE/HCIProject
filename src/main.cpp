#include <Arduino.h>
#include "integrations/ancs/ancs.h" 
#include "integrations/googleCalendar/googleCalendar.h"
#include <WiFi.h>

#define WIFI_SSID "dev"
#define WIFI_PWD "testtest"

#define CALENDAR_API_KEY "ya29.a0AeXRPp6IfLZ-MGruFLemxTiW08mBr2SxihIISkyKGAvgKRCfrSnbBDYS-jjDEie8yyWyE0_IlNqXzdUBmocm3Eee4d5WPdWL3FbjWQdMMRQZokE8Jtg4is-a0jwjjcgvNWq8cWqIzScXuzDpABvttMXHHyvZp8awpstlGTBcaCgYKAQ8SARMSFQHGX2MiJf26HzZW-eXwG52-Ll-t0w0175"
#define CALENDAR_ID "c_bec6564656aa30cdc4525b876e527e9df464a013eaef033225ce2ae2fca47580@group.calendar.google.com"

static char LOG_TAG[] = "Main";

GoogleCalendarAPI calendar(CALENDAR_API_KEY, CALENDAR_ID);

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

    int numEvents;
    CalendarEvent* events = calendar.pollEvents(&numEvents);
}

void loop()
{

}
