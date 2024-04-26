#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 5
#define RST_PIN 4

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC_1[4]; // Array to store Student 1's card ID
byte nuidPICC_2[4]; // Array to store Student 2's card ID

void setup() {
  Serial.begin(115200);

lcd.init();
  lcd.backlight();
  lcd.clear();

  SPI.begin();          // Init SPI bus
  rfid.PCD_Init();      // Init MFRC522

  // Initialize keys
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scans the MIFARE Classic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  lcd.clear();
  lcd.setCursor(0, 0);
lcd.print("tag scaning:");
    
  // Initialize card IDs to empty
  memset(nuidPICC_1, 0, sizeof(nuidPICC_1));
  memset(nuidPICC_2, 0, sizeof(nuidPICC_2));
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been read
  if (!rfid.PICC_ReadCardSerial())
    return;

  // Check if the card is of MIFARE Classic type
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    lcd.setCursor(0,0);
    lcd.print("invalid tag!!!");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
    return;
  }

  // Check if the card matches Student 1's card
  if (memcmp(rfid.uid.uidByte, nuidPICC_1, sizeof(nuidPICC_1)) == 0) {
    Serial.println(F("Student 1 detected."));
    lcd.clear();
    lcd.print("tag 1 detected.");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
  }
  // Check if the card matches Student 2's card
  else if (memcmp(rfid.uid.uidByte, nuidPICC_2, sizeof(nuidPICC_2)) == 0) {
    Serial.println(F("Student 2 detected."));
    lcd.clear();
    lcd.print("tag 2 detected.");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
  }
  // If the card doesn't match either, it's a new card
  else {
    Serial.println(F("Unknown card detected."));
    // Assign the new card ID to the first available slot
    if (memcmp(nuidPICC_1, "\0\0\0\0", sizeof(nuidPICC_1)) == 0) {
      memcpy(nuidPICC_1, rfid.uid.uidByte, sizeof(nuidPICC_1));
      Serial.println(F("Recorded as Student 1."));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("unkown tag:");
      lcd.setCursor(0, 1);
      lcd.print("tag 1 added.");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
    }
    else if (memcmp(nuidPICC_2, "\0\0\0\0", sizeof(nuidPICC_2)) == 0) {
      memcpy(nuidPICC_2, rfid.uid.uidByte, sizeof(nuidPICC_2));
      Serial.println(F("Recorded as Student 2."));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("unkown tag:");
      lcd.setCursor(0, 1);
      lcd.print("tag 2 added.");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
    }
    else {
      Serial.println(F("No more space for new cards."));
      lcd.clear();
      lcd.print("space full !!!");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
    }
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
