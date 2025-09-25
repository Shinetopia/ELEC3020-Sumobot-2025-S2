#include <Arduino.h>
#include <TFT_eSPI.h>

#define PSD_PIN 12

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(9600);
  pinMode(PSD_PIN, INPUT);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
}

void loop() {
  int reading = analogRead(PSD_PIN);
  float percentage = (float)reading / 4095.0f;
  float scaledVal = percentage * 3.3f;
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 50);
  tft.printf("PSD voltage: %0.2f", scaledVal);
  delay(500);
}
