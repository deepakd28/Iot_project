#include "EEPROM.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\nTesting EEPROM Library\n");
  if (!EEPROM.begin(1000)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  int address = 0;


  String name = "Deepak";
  EEPROM.writeString(address, name);
  address += name.length() + 1;
  String blood_type = "o+ve";
  EEPROM.writeString(address, blood_type);
  address += blood_type.length() + 1;
  String contact = "8217811621";
  EEPROM.writeString(address, contact);


  // See also the general purpose writeBytes() and readBytes() for BLOB in EEPROM library
  EEPROM.commit();
  address = 0;
  
                                   // Clear Value
  

  Serial.println(EEPROM.readString(address));
  address += name.length() + 1;
  Serial.println(EEPROM.readString(address));
  address += blood_type.length() + 1;
  Serial.println(EEPROM.readString(address));
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
