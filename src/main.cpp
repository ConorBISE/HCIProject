/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "ancs/ancs.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 2
#define TFT_CS 15

#define TFT_MOSI 13
#define TFT_CLK 14
#define TFT_RST -1
#define TFT_MISO -1

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup()
{
  Serial.begin(115200);
  Serial.println("ILI9341 Test!");

  delay(1000);

  tft.begin();

  delay(1000);

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setRotation(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(5);
  tft.println("Hello World!");
}

void loop(void)
{
}
