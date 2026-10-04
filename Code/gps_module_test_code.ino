#include <TinyGPS++.h>

// Hardware UART Pins for ESP32-S3 N16R8
#define RXD1 18  // ESP32-S3 RX pin -> Connect to NEO-M8N TX
#define TXD1 17  // ESP32-S3 TX pin -> Connect to NEO-M8N RX

TinyGPSPlus gps;

void setup() {
  // Serial Monitor Baud Rate
  Serial.begin(115200);
  delay(1000); // Small pause for Serial initialization

  // Start Serial1 communication with GPS module at 9600 baud
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);

  Serial.println("=========================================");
  Serial.println("ESP32-S3 N16R8 + NEO-M8N GPS Initializing...");
  Serial.println("=========================================");
}

void loop() {
  // Feed incoming GPS data to TinyGPSPlus parser
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }

  // Display location details once valid sentences are processed
  if (gps.location.isUpdated()) {
    displayGPSInfo();
  }

  // Warning check if no data is received on GPIO 18
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("[Warning] No GPS data detected. Check VCC/GND power and TX/RX wiring!");
    delay(2000);
  }
}

void displayGPSInfo() {
  Serial.print("Latitude: ");
  Serial.print(gps.location.lat(), 6);
  Serial.print(" | Longitude: ");
  Serial.print(gps.location.lng(), 6);

  Serial.print(" | Satellites: ");
  if (gps.satellites.isValid()) {
    Serial.print(gps.satellites.value());
  } else {
    Serial.print("0");
  }

  Serial.print(" | Altitude: ");
  if (gps.altitude.isValid()) {
    Serial.print(gps.altitude.meters());
    Serial.println("m");
  } else {
    Serial.println("N/A");
  }
}