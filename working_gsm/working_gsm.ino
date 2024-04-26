#include <SoftwareSerial.h>

SoftwareSerial SIM900A(2,3);

// Variables to store user data
char name[50];
char blood_group[10];
char contact_no[15];

// Flag to indicate if override is received
bool overrideReceived = false;

void setup() {
  // Initialize serial communication with GSM module
  gsmSerial.begin(9600);  // Set the baud rate of GSM Module
  delay(1000);
  //  sendSMS("helloo");
   Serial.begin(9600);
}

void loop() {
  if (!overrideReceived) {
    // If override not received, check for incoming SMS
    checkIncomingSMS();
  } else {
    // If override received, parse the SMS
    parseSMS();
  }
  
}

void checkIncomingSMS() {
  if (gsmSerial.available() > 0) {
    String msg = gsmSerial.readStringUntil('\n');
    if (msg.startsWith("override")) {
      overrideReceived = true;
      sendSMS("Override received. Send data in the format: Name Blood_group Contact_no");
    }
  }
}

void parseSMS() {
  if (gsmSerial.available() > 0) {
    String msg = gsmSerial.readStringUntil('\n');
    int space1 = msg.indexOf(' ');
    int space2 = msg.indexOf(' ', space1 + 1);
    if (space1 != -1 && space2 != -1) {
      msg.toCharArray(name, space1 + 1);
      msg.substring(space1 + 1, space2).toCharArray(blood_group, space2 - space1);
      msg.substring(space2 + 1).toCharArray(contact_no, msg.length() - space2);
      sendSMS("Data received successfully!");
      // Reset override flag
      overrideReceived = false;
      Serial.print(name);
      Serial.print(blood_group);
      Serial.print(contact_no);
    }
  }
}

void sendSMS(const char* message) {
  gsmSerial.println("AT+CMGF=1"); // Set the GSM module to Text Mode
  delay(1000);
  
  // Replace the destination phone number with the appropriate one
  gsmSerial.println("AT+CMGS=\"+91xxxxxxxxxxxx\"\r");
  delay(1000);
  
  // Send the message
  gsmSerial.println(message);
  delay(100);
  
  gsmSerial.println((char)26); // ASCII code of CTRL+Z for ending the SMS
  delay(1000);
}