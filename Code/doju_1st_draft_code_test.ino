#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TinyGPS++.h>

// ESP32-S3 Display Pins (Confirmed Working)
#define TFT_CS   10
#define TFT_DC   11
#define TFT_RST  14
#define TFT_MOSI 13
#define TFT_SCLK 12
#define TFT_MISO -1

// GPS UART Pins (Hardware Serial 1)
#define GPS_RX 18 // ESP32 RX1 -> GPS TX
#define GPS_TX 17 // ESP32 TX1 -> GPS RX

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

// Moira Bounding Box (Lat/Lon limits)
const float MIN_LAT = 15.5880;
const float MAX_LAT = 15.6120;
const float MIN_LON = 73.8250;
const float MAX_LON = 73.8550;

struct LineSegment {
  float lat1, lon1;
  float lat2, lon2;
};

// Simplified Vector Map for Moira Roads
const LineSegment moiraMap[] = {
  // Moira Main Road
  {15.5950, 73.8300, 15.5990, 73.8350},
  {15.5990, 73.8350, 15.6020, 73.8400},
  // Church Road / Bambordem
  {15.6020, 73.8400, 15.6050, 73.8430},
  {15.6050, 73.8430, 15.6080, 73.8480},
  // Atafondem Link Bridge
  {15.5920, 73.8320, 15.5900, 73.8280}
};

const int mapSegmentCount = sizeof(moiraMap) / sizeof(moiraMap[0]);

// PSRAM Canvas for smooth drawing
GFXcanvas16 *canvas;

// Convert GPS coordinates to screen pixel positions
void gpsToPixel(float lat, float lon, int &x, int &y) {
  x = (int)(((lon - MIN_LON) / (MAX_LON - MIN_LON)) * 240.0);
  y = (int)(((MAX_LAT - lat) / (MAX_LAT - MIN_LAT)) * 320.0);
  
  x = constrain(x, 0, 239);
  y = constrain(y, 0, 319);
}

// Render complete frame offscreen, then push to display
void renderFrame(int playerX, int playerY, bool gpsValid) {
  // Clear canvas background
  canvas->fillScreen(ILI9341_BLACK);

  // 1. Draw Moira Road Lines
  for (int i = 0; i < mapSegmentCount; i++) {
    int x1, y1, x2, y2;
    gpsToPixel(moiraMap[i].lat1, moiraMap[i].lon1, x1, y1);
    gpsToPixel(moiraMap[i].lat2, moiraMap[i].lon2, x2, y2);
    canvas->drawLine(x1, y1, x2, y2, ILI9341_DARKGREEN);
  }

  // 2. Draw GPS Position Marker
  if (gpsValid) {
    canvas->fillCircle(playerX, playerY, 4, ILI9341_RED);
    canvas->drawCircle(playerX, playerY, 7, ILI9341_WHITE);
  } else {
    // Status message while waiting for satellite fix
    canvas->setCursor(10, 10);
    canvas->setTextColor(ILI9341_YELLOW);
    canvas->setTextSize(1);
    canvas->print("Acquiring GPS Fix...");
  }

  // Draw full frame in one single SPI pass
  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 240, 320);
}

void setup() {
  Serial.begin(115200);
  
  // Start GPS Hardware Serial on GPIO 18 (RX) & GPIO 17 (TX)
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  // Initialize SPI with tested working pins
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

  tft.begin();
  tft.setRotation(0);

  // Allocate 153.6 KB Canvas in PSRAM
  canvas = new GFXcanvas16(240, 320);

  // Initial draw before GPS gets a fix
  renderFrame(0, 0, false);
}

void loop() {
  // Parse incoming NMEA sentences from GPS module
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        int x, y;
        gpsToPixel(gps.location.lat(), gps.location.lng(), x, y);
        renderFrame(x, y, true);
      }
    }
  }
}