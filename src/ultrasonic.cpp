#define ULTRASONIC
#ifndef ULTRASONIC

#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
const int activate_pin = 1;
const int reciver_pin = 2;

void setup() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(3);
  pinMode(activate_pin, OUTPUT);
  pinMode(reciver_pin, INPUT);
  digitalWrite(activate_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(activate_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(activate_pin, LOW);

  int duration = pulseIn(reciver_pin, HIGH);
  tft.drawNumber(duration, 0, 0);
}

void loop() {
}

#endif