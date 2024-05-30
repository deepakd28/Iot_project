#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int crashButtonPin = 25;  // Replace with the GPIO pin for the crash button
const int cancelButtonPin = 26; // Replace with the GPIO pin for the cancel button

bool inCrashAlertState = false;

void setup() {
  lcd.init();          // Initialize the LCD
  lcd.backlight();     // Turn on the LCD backlight
  lcd.clear();         // Clear the LCD screen

  // Display the "Good morning" message on the first line
  lcd.setCursor((16 - 12) / 2, 0); // Center position for a 16x2 LCD
  lcd.print("Good morning");

  delay(1000); // Wait for a moment
}

void loop() {
  // Check if the crash button is pressed
  if (digitalRead(crashButtonPin) == HIGH && !inCrashAlertState) {
    inCrashAlertState = true;

    lcd.clear();
    lcd.setCursor((16 - 14) / 2, 0);
    lcd.print("Crash alert!!!");

    lcd.setCursor((16 - 16) / 2, 1);
    lcd.blink();
    lcd.print("Press to cancel");

    debounceDelay(500); // Debounce delay for 0.5 seconds

    lcd.noBlink(); // Stop blinking
  }

  // Check if the cancel button is pressed
  if (digitalRead(cancelButtonPin) == HIGH && inCrashAlertState) {
    inCrashAlertState = false;

    // Display the "Good morning" message
    lcd.clear();
    lcd.setCursor((16 - 12) / 2, 0);
    lcd.print("Good morning");
    debounceDelay(1000); // Debounce delay for 1 second
  }

  delay(10); // Add a small delay to avoid button bouncing
}

void debounceDelay(unsigned long ms) {
  unsigned long currentMillis = millis();
  while (millis() - currentMillis < ms) {
    delay(10); // Adjust the delay interval based on the responsiveness you need
  }
}
