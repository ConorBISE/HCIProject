#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <WiFi.h>
#include <FastLED.h>

#include "integrations/ancs/ancs.h"
#include "integrations/googleCalendar/googleCalendar.h"
#include "screens/screenEvent.h"
#include "screens/screen.h"
#include "screens/timeScreen.h"
#include "screens/weatherScreen.h"
#include "screens/pomodoroScreen.h"

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
#define LEFT_TOUCH_THRESHOLD 32

#define RIGHT_TOUCH_PIN T5
#define RIGHT_TOUCH_THRESHOLD 30

#define TOP_TOUCH_PIN T7
#define TOP_TOUCH_THRESHOLD 42

volatile boolean _leftTouched = false;
volatile boolean _rightTouched = false;
volatile boolean _topTouched = false;

Screen *SCREENS[] = {
    new TimeScreen(&tft, leds),
    new WeatherScreen(&tft, leds),
    new PomodoroScreen(&tft, leds)};

const int numScreens = sizeof(SCREENS) / sizeof(Screen *);

int activeScreenIndex = 0;

void leftTouchDetected()
{
  _leftTouched = true;
}

void rightTouchDetected()
{
  _rightTouched = true;
}

void topTouchDetected()
{
  _topTouched = true;
}

boolean isNotificationShowing = false;
long notificationDisappearTime = 0;

void showNotification(String title, String subtitle, CRGB color)
{
  isNotificationShowing = true;
  notificationDisappearTime = millis() + 10 * 1000;

  tft.setRotation(1);

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  int16_t x1, y1;
  uint16_t w, h;

  // Clear the entire screen
  tft.fillScreen(ILI9341_BLACK);

  // Measure the title text
  tft.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
  int16_t titleX = (tft.width() - w) / 2;
  int16_t titleY = (tft.height() / 2) - h - 5; // Slightly above center

  // Measure the subtitle text
  int16_t x2, y2;
  uint16_t w2, h2;
  tft.getTextBounds(subtitle, 0, 0, &x2, &y2, &w2, &h2);
  int16_t subtitleX = (tft.width() - w2) / 2;
  int16_t subtitleY = (tft.height() / 2) + 5; // Slightly below center

  // Calculate the bounding rectangle for both title and subtitle
  int16_t rectX = min(titleX - 5, subtitleX - 5);
  int16_t rectY = titleY - 5;
  int16_t rectW = max(titleX + w + 5, subtitleX + w2 + 5) - rectX;
  int16_t rectH = (subtitleY + h2 + 5) - rectY;

  // Draw a single rectangle around both
  tft.drawRect(rectX, rectY, rectW, rectH, ILI9341_WHITE);

  // Draw the title
  tft.setCursor(titleX, titleY);
  tft.println(title);

  // Draw the subtitle
  tft.setCursor(subtitleX, subtitleY);
  tft.println(subtitle);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = color.nscale8(50);
  }

  FastLED.show();

  delay(1000);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }

  FastLED.show();
}

void notifiationCallback(ancs::Notification *notification)
{
  Serial.print("New notification!: ");
  Serial.println(notification->title);

  showNotification(notification->title, notification->message, CRGB(179, 87, 7));
}

void startWifi()
{
  ESP_LOGI(LOG_TAG, "Connecting to wifi");

  WiFi.begin(WIFI_SSID, WIFI_PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    ESP_LOGI(LOG_TAG, ".");
    delay(1000);
  }

  ESP_LOGI(LOG_TAG, "\nConnected to wifi!");

  configTime(0, 0, "pool.ntp.org");
}

void setup()
{
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }

  FastLED.show();

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  touchAttachInterrupt(LEFT_TOUCH_PIN, leftTouchDetected, LEFT_TOUCH_THRESHOLD);
  touchAttachInterrupt(RIGHT_TOUCH_PIN, rightTouchDetected, RIGHT_TOUCH_THRESHOLD);
  touchAttachInterrupt(TOP_TOUCH_PIN, topTouchDetected, TOP_TOUCH_THRESHOLD);

  server.setNotificationCallback(notifiationCallback);
  server.run();

  startWifi();

  SCREENS[activeScreenIndex]->onEnter();
}

boolean leftPreviouslyTouched = false;
boolean rightPreviouslyTouched = false;
boolean topPreviouslyTouched = false;

boolean processButtonEvents(ButtonEvent &event)
{
  boolean haveEvent = false;

  if (_leftTouched && !leftPreviouslyTouched)
  {
    haveEvent = true;
    event = {ButtonPosition::Left, ButtonState::Pressed};
    leftPreviouslyTouched = true;
  }
  else if (!_leftTouched && leftPreviouslyTouched)
  {
    haveEvent = true;
    event = {ButtonPosition::Left, ButtonState::Released};
    leftPreviouslyTouched = false;
  }

  if (_rightTouched && !rightPreviouslyTouched)
  {
    haveEvent = true;
    event = {ButtonPosition::Right, ButtonState::Pressed};
    rightPreviouslyTouched = true;
  }
  else if (!_rightTouched && rightPreviouslyTouched)
  {
    haveEvent = true;
    event = {ButtonPosition::Right, ButtonState::Released};
    rightPreviouslyTouched = false;
  }

  if (_topTouched && !topPreviouslyTouched)
  {
    haveEvent = true;
    event = {ButtonPosition::Top, ButtonState::Pressed};
    topPreviouslyTouched = true;
  }
  else if (!_topTouched && topPreviouslyTouched)
  {
    haveEvent = true;
    event = {ButtonPosition::Top, ButtonState::Released};
    topPreviouslyTouched = false;
  }

  // Reset the touch flags for the next loop iteration
  _leftTouched = false;
  _rightTouched = false;
  _topTouched = false;

  return haveEvent;
}

long nextCalendarCheck = 0;

void loop()
{
  ButtonEvent event;
  boolean haveEvent = processButtonEvents(event);

  if (haveEvent)
  {
    if (event.position == ButtonPosition::Left && event.state == ButtonState::Pressed)
    {
      Serial.println("Left press");
      activeScreenIndex--;
      if (activeScreenIndex < 0)
        activeScreenIndex = numScreens - 1;

      SCREENS[activeScreenIndex]->onEnter();
    }

    if (event.position == ButtonPosition::Right && event.state == ButtonState::Pressed)
    {
      Serial.println("Right press");
      activeScreenIndex++;
      if (activeScreenIndex >= numScreens)
        activeScreenIndex = 0;

      SCREENS[activeScreenIndex]->onEnter();
    }
  }

  if (!isNotificationShowing)
  {
    SCREENS[activeScreenIndex]->update(haveEvent ? &event : nullptr);
  }
  else if (millis() > notificationDisappearTime)
  {
    isNotificationShowing = false;
    SCREENS[activeScreenIndex]->onEnter();
  }

  if (millis() > nextCalendarCheck)
  {
    int numEvents;
    CalendarEvent *events = calendar.pollEvents(&numEvents);

    for (int i = 0; i < numEvents; i++)
    {
      CalendarEvent *event = &events[i];

      time_t startTime = mktime(&event->startTime);
      tm now;
      getLocalTime(&now);
      time_t now_time_t = mktime(&now);

      auto timediff = difftime(startTime, now_time_t);

      if (timediff > 0 && timediff < EVENT_ALERT_TIME)
      {
        showNotification("Event in 5 minutes!", event->name, CRGB(179, 87, 7));
      }

      nextCalendarCheck = millis() + 30 * 1000;
    }
  }
  else
  {
    delay(100);
  }
}