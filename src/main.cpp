#include <Arduino.h>
#include <TFT_eSPI.h>

#define BTN_LEFT 0
#define BTN_RIGHT 14 

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(9600);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(10, 50);
  tft.print("Hello, world");
}

void loop() {

}