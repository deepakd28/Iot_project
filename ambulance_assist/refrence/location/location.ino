#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);
HardwareSerial gpsSerial(0); // Connect GPS module to Serial2 on ESP32

TinyGPSPlus gps;

unsigned long lastMillis = 0;
const unsigned long interval = 10000; // Update every 10 seconds
// #define RXD2 16
// #define TXD2 17
// HardwareSerial gpsSerial(1);

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600); // GPS module default baud rate

  lcd.init();
  lcd.backlight();
  lcd.clear();


/// Wait for a moment
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      displayGPSInfo();
    }
  }

  // Check if it's time to get new coordinates
  if (millis() - lastMillis >= interval) {
    lastMillis = millis();
    getCoordinates();
  }
}

void displayGPSInfo() {
  lcd.setCursor(0, 0);
  lcd.print("Lat: ");
  lcd.print(gps.location.lat(), 8); // Displaying more decimal places
  lcd.print("   ");

  lcd.setCursor(0, 2);
  lcd.print("Lon: ");
  lcd.print(gps.location.lng(), 8); // Displaying more decimal places
  lcd.print("   ");
}

void getCoordinates() {
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.print(gps.location.lat(), 8);
    Serial.print(" Longitude: ");
    Serial.println(gps.location.lng(), 8);

    // Generate Google Maps link
    String mapLink = "https://www.google.com/maps/place/" + String(gps.location.lat(), 8) + "," + String(gps.location.lng(), 8);
    
    Serial.print("Google Maps Link: ");
    Serial.println(mapLink);

    displayGPSInfo(); // Display the coordinates on the LCD
  } else {
    Serial.println("No GPS data available");
  }
}