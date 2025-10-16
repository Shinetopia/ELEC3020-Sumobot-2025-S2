#define PSD
#ifndef PSD
#include <TFT_eSPI.h>

#include <iomanip>
#include <sstream>
TFT_eSPI tft = TFT_eSPI();
int reflectance = 0;

void setup() {
  tft.init();
  tft.fillScreen(TFT_WHITE);
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLACK, TFT_WHITE, true);

  tft.drawString("hi", 0, 30);
}

void loop() {
  reflectance = analogRead(16);
  std::stringstream stream;
  stream << std::setw(5) << std::setfill('0') << reflectance;

  tft.drawString(stream.str().c_str(), 0, 0);
}

#endif