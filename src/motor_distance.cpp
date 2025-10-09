#define MOTOR
#ifndef MOTOR

#include <Arduino.h>
#include <TFT_eSPI.h>

#include <iomanip>
#include <sstream>

const int left = 0;
const int right = 14;
const int motor_drive = 1;
const int psd_in = 2;

TFT_eSPI tft = TFT_eSPI();

void main_loop() {
  int mode = 0;
  int left_previous = 1;
  int right_previous = 1;
  int left_now = 1;
  int right_now = 1;
  int left_last_pressed = 0;
  int right_last_pressed = 0;
  while (true) {
    left_now = digitalRead(left);
    right_now = digitalRead(right);
    if (!left_now && left_previous && millis() - left_last_pressed > 20) {
      left_previous = left_now;
      left_last_pressed = millis();
      mode = (mode + 2) % 3;
    } else if (left_now && !left_previous &&
               millis() - left_last_pressed > 20) {
      left_previous = left_now;
    }
    if (!right_now && right_previous && millis() - right_last_pressed > 20) {
      right_previous = right_now;
      right_last_pressed = millis();
      mode = (mode + 1) % 3;
    } else if (right_now && !right_previous &&
               millis() - right_last_pressed > 20) {
      right_previous = right_now;
    }
    int value = analogRead(psd_in);
    std::stringstream stream;
    tft.setCursor(0, 30);
    stream << std::setw(4) << std::setfill('0') << value;
    tft.println(stream.str().c_str());
    switch (mode) {
      case 0:
        tft.drawString(" 10%", 0, 0);
        ledcWrite(0, 25);
        break;
      case 1:
        tft.drawString(" 50%", 0, 0);
        ledcWrite(0, 128);
        break;
      case 2:
        tft.drawString("100%", 0, 0);
        ledcWrite(0, 256);
        break;
    }
  }
}

void setup() {
  Serial.begin(9600);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 50);
  ledcSetup(0, 2000, 8);
  ledcAttachPin(motor_drive, 0);
  main_loop();
}

void loop() {
}

#endif