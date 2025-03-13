#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include "integrations/ancs/ancs.h" 
#include "integrations/googleCalendar/googleCalendar.h"
#include <WiFi.h>

#include <FastLED.h>

#define NUM_LEDS 10
#define DATA_PIN 25
CRGB leds[NUM_LEDS];

#define WIFI_SSID "dev"
#define WIFI_PWD "testtest"

#define CALENDAR_ID "c_bec6564656aa30cdc4525b876e527e9df464a013eaef033225ce2ae2fca47580@group.calendar.google.com"
#define EVENT_ALERT_TIME 60 * 5

static char LOG_TAG[] = "Main";

GoogleCalendarAPI calendar(CALENDAR_ID);  
ancs::ANCSServer server;

#define TFT_DC 2
#define TFT_CS 15

#define TFT_MOSI 13
#define TFT_CLK 14
#define TFT_RST -1
#define TFT_MISO -1

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define LEFT_TOUCH_PIN T0
#define RIGHT_TOUCH_PIN T5
#define TOP_TOUCH_PIN T7
#define TOUCH_THRESHOLD 30

volatile boolean _leftTouched = false;
volatile boolean _rightTouched = false;
volatile boolean _topTouched = false;

void leftTouchDetected() {
  _leftTouched = true;
}

void rightTouchDetected() {
  _rightTouched = true;
}

void topTouchDetected() {
  _topTouched = true;
}

void notifiationCallback(ancs::Notification* notification) {
    Serial.print("New notification!: ");
    Serial.println(notification->title);
}

void setup()
{
    Serial.begin(115200);
    
    FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS); 
    
    tft.begin();
    delay(100);
    tft.fillScreen(ILI9341_RED);

    // touchAttachInterrupt(LEFT_TOUCH_PIN, leftTouchDetected, TOUCH_THRESHOLD);
    // touchAttachInterrupt(RIGHT_TOUCH_PIN, rightTouchDetected, TOUCH_THRESHOLD);
    // touchAttachInterrupt(TOP_TOUCH_PIN, topTouchDetected, TOUCH_THRESHOLD);

    // server.setNotificationCallback(notifiationCallback);
    // server.run();

    // ESP_LOGI(LOG_TAG, "Connecting to wifi");

    // WiFi.begin(WIFI_SSID, WIFI_PWD);

    // while (WiFi.status() != WL_CONNECTED) {
    //     ESP_LOGI(LOG_TAG, ".");
    //     delay(1000);
    // }

    // ESP_LOGI(LOG_TAG, "\nConnected to wifi!");

    // configTime(0, 0, "pool.ntp.org");

    // touchSetCycles(0x2000, 0x1000);
}

void loop()
{

  for (int i = 0; i < 2; i++) {
    leds[i] = CRGB::White;
  }

  FastLED.show();
  delay(100);

  for (int i = 0; i < 2; i++) {
    leds[i] = CRGB::Black;
  }

  FastLED.show();
  delay(100);



  // Serial.print(touchRead(TOP_TOUCH_PIN));
  // Serial.print(" ");
  // Serial.print(touchRead(LEFT_TOUCH_PIN));
  // Serial.print(" ");
  // Serial.println(touchRead(RIGHT_TOUCH_PIN));

  // delay(100);
  // if (_leftTouched) {
  //   _leftTouched = false;
  //   Serial.println("Left press");
  // }

  // if (_rightTouched) {
  //   _rightTouched = false;
  //   Serial.println("Right press");
  // }

  // if (_topTouched) {
  //   _topTouched = false;
  //   Serial.println("Top press");
  // }

  // int numEvents;
  // CalendarEvent* events = calendar.pollEvents(&numEvents);

  // for (int i = 0; i < numEvents; i++) {
  //   CalendarEvent* event = &events[i];

  //   time_t startTime = mktime(&event->startTime);
  //   tm now;
  //   getLocalTime(&now);
  //   time_t now_time_t = mktime(&now);

  //   auto timediff = difftime(startTime, now_time_t);

  //   if (timediff > 0 && timediff < EVENT_ALERT_TIME) {
  //     Serial.println("Event upcoming!");
  //     Serial.println(event->name);
  //   }
  // }

  // delay(1000 * 10);
}