#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 5
#define RST_PIN 4

#define SWITCH_PIN 2

const int powled = 17;
const int conled = 16;
const int satled = 15;


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

byte Lane_1[4]; // Array to store Student 1's card ID
byte Lane_2[4]; // Array to store Student 2's card ID
byte Lane_3[4]; // Array to store Student 3's card ID
byte Lane_4[4]; // Array to store Student 4's card ID

uint8_t receiverAdd[] = {0x40, 0x22, 0xd8, 0x4f, 0x0e, 0x05};
esp_now_peer_info_t peerInfo;

int data = 0;
String sta;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS)
    sta = "Delivery Success";
  else
    sta = "Delivery Fail";
  Serial.println(sta);
  Serial.println("");
  digitalWrite(satled, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(satled, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(powled, OUTPUT);
  pinMode(conled, OUTPUT);
  pinMode(satled, OUTPUT);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, receiverAdd, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  SPI.begin();         // Init SPI bus
  rfid.PCD_Init();     // Init MFRC522

  // Initialize keys
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scans the MIFARE Classic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("tag scanning:");

  memset(Lane_1, 0, sizeof(Lane_1));
  memset(Lane_2, 0, sizeof(Lane_2));
  memset(Lane_3, 0, sizeof(Lane_3));
  memset(Lane_4, 0, sizeof(Lane_4));

  pinMode(SWITCH_PIN, INPUT); // Set switch pin as input with internal pull-up resistor
}

void loop() {
  // Read the state of the switch
  int switchState = digitalRead(SWITCH_PIN);

  // Check if the switch is turned on
  if (switchState == HIGH) {
    digitalWrite(powled, HIGH);  // turn the LED on (HIGH is the voltage level)
    scanRFID();
    delay(1000);
    digitalWrite(conled, HIGH);
  }
  else {
  digitalWrite(powled, LOW);
    digitalWrite(conled, LOW);
    
  }
}

void scanRFID() {
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
  if (memcmp(rfid.uid.uidByte, Lane_1, sizeof(Lane_1)) == 0) {
    Serial.println(F("Lane 1 detected."));
    lcd.clear();
    lcd.print("tag 1 detected.");


    data=1;
    esp_now_send(receiverAdd, (uint8_t *) &data, sizeof(data));
    Serial.print("lane no:");
    Serial.println(data);
    



    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
  }
  // Check if the card matches Student 2's card
  else if (memcmp(rfid.uid.uidByte, Lane_2, sizeof(Lane_2)) == 0) {
    Serial.println(F("Lane 2 detected."));
    lcd.clear();
    lcd.print("tag 2 detected.");

    data=2;
    esp_now_send(receiverAdd, (uint8_t *) &data, sizeof(data));
    Serial.print("lane no:");
    Serial.println(data);
    


    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
  }
  else if (memcmp(rfid.uid.uidByte, Lane_3, sizeof(Lane_3)) == 0) {
    Serial.println(F("Lane 3 detected."));
    lcd.clear();
    lcd.print("tag 3 detected.");

    data=3;
    esp_now_send(receiverAdd, (uint8_t *) &data, sizeof(data));
    Serial.print("lane no:");
    Serial.println(data);
    


    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
  }
  else if (memcmp(rfid.uid.uidByte, Lane_4, sizeof(Lane_4)) == 0) {
    Serial.println(F("Lane 4 detected."));
    lcd.clear();
    lcd.print("tag 4 detected.");

    data=4;
    esp_now_send(receiverAdd, (uint8_t *) &data, sizeof(data));
    Serial.print("lane no:");
    Serial.println(data);
    


    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
  }
  // If the card doesn't match either, it's a new card
  else {
    Serial.println(F("Unknown card detected."));
    // Assign the new card ID to the first available slot
    if (memcmp(Lane_1, "\0\0\0\0", sizeof(Lane_1)) == 0) {
      memcpy(Lane_1, rfid.uid.uidByte, sizeof(Lane_1));
      Serial.println(F("Recorded as Lane 1."));
      Serial.println("");

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
    else if (memcmp(Lane_2, "\0\0\0\0", sizeof(Lane_2)) == 0) {
      memcpy(Lane_2, rfid.uid.uidByte, sizeof(Lane_2));
      Serial.println(F("Recorded as Lane 2."));
      Serial.println("");
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
    else if (memcmp(Lane_3, "\0\0\0\0", sizeof(Lane_3)) == 0) {
      memcpy(Lane_3, rfid.uid.uidByte, sizeof(Lane_3));
      Serial.println(F("Recorded as Lane 3."));
      Serial.println("");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("unkown tag:");
      lcd.setCursor(0, 1);
      lcd.print("tag 3 added.");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
    }
    else if (memcmp(Lane_4, "\0\0\0\0", sizeof(Lane_4)) == 0) {
      memcpy(Lane_4, rfid.uid.uidByte, sizeof(Lane_4));
      Serial.println(F("Recorded as Lane 4."));
      Serial.println("");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("unkown tag:");
      lcd.setCursor(0, 1);
      lcd.print("tag 4 added.");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("tag scaning:");
    }
    else {
      Serial.println(F("No more space for new cards."));
      Serial.println("");
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




void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
