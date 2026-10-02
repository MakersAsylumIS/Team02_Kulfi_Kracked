#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); 

void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.init();
  tft.setRotation(1); // 1 = Landscape orientation (320x240)
  tft.fillScreen(TFT_BLACK);

  // Header Text
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.println("ESP32 + 2.4\" TFT Display");

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(20, 50);
  tft.println("Status: ONLINE & WORKING!");

  // Graphic Test Elements
  tft.drawRect(20, 80, 280, 100, TFT_BLUE);
  tft.fillRect(35, 95, 40, 40, TFT_RED);
  tft.fillCircle(120, 115, 20, TFT_YELLOW);
  tft.drawTriangle(180, 135, 200, 95, 220, 135, TFT_CYAN);
}

void loop() {
  static uint16_t counter = 0;
  
  // Live counter display
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString("Count: " + String(counter) + "  ", 20, 195);

  counter++;
  delay(1000);
}