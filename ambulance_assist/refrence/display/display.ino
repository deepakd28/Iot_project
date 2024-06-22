#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
int i=0;

void setup() {
  lcd.init();          // Initialize the LCD
  lcd.backlight();     // Turn on the LCD backlight
  lcd.clear();         // Clear the LCD screen

  
  lcd.setCursor(0, 0); // Center position for a 16x2 LCD
  lcd.print("Good morning");

  delay(1000); // Wait for a moment
}

void loop() {
  lcd.setCursor(0, 1); // Center position for a 16x2 LCD
  lcd.print(i);
  i=i+1;
  delay(1000);
}


