#include <Wire.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>


LiquidCrystal_I2C lcd(0x27, 16, 2); 
HardwareSerial gpsSerial(0);
HardwareSerial mySerial(2);
TinyGPSPlus gps;


unsigned long lastMillis = 0;
const unsigned long interval = 10000; 
const int MPU_addr = 0x68;   // I2C address of the MPU-6050
const int buttonPin = 2;
const int Buzzerpin = 5;     // Pin for the button
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
boolean fall = false;         // stores if a fall has occurred
boolean trigger1 = false;     // stores if first trigger (lower threshold) has occurred
boolean trigger2 = false;     // stores if second trigger (upper threshold) has occurred
boolean trigger3 = false;     // stores if third trigger (orientation change) has occurred
byte trigger1count = 0;       // stores the counts past since trigger 1 was set true
byte trigger2count = 0;       // stores the counts past since trigger 2 was set true
byte trigger3count = 0;       // stores the counts past since trigger 3 was set true
int angleChange = 0;
boolean buttonPressed = false;
int timer= 10;
int flag =0; // flag to indicate if the button is pressed
String command;
String name;
String blood_group;
String contact_no;
String c_no;
// String temp_stat;

// Flag to indicate if override is received
bool overrideReceived = false;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pinMode(buttonPin, INPUT);
  pinMode(Buzzerpin, OUTPUT);  // Set button pin as input with internal pull-up resistor
  Serial.println("Wrote to IMU");
  lcd.init();                       // Initialize the LCD
  lcd.backlight();                  // Turn on the backlight
  lcd.clear();
normalmode();
gpsSerial.begin(9600);
mySerial.begin(9600); 
sendSMSamb("hello");

}
void normalmode(){
lcd.clear();
lcd.setCursor(3, 0);
lcd.print("ACS SYSTEM");
if(name!=0){
  lcd.setCursor(0, 1);
  lcd.print(name);
}
timer=10;
flag=0;
}
void crashed() {
  Serial.println("crashed!!!!");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("crashed!!");
  
  while (!buttonPressed && flag!=1) {  // Stay in this loop until button is pressed
    Serial.println("Waiting for button press...");
    lcd.setCursor(0,1);
    lcd.print("hold to cancel");
    lcd.setCursor(15, 0);
  lcd.print(timer);
  timer--;
digitalWrite(Buzzerpin, HIGH);
  if(timer==0){
    crashcall();
    flag=1;
  } 
    if (digitalRead(buttonPin) == HIGH) {
      buttonPressed = true;
      Serial.println("Button pressed");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("cancelled.");
    delay(4500);
      normalmode();
    }
    delay(1000);
digitalWrite(Buzzerpin, LOW);
  }
  buttonPressed = false;

}

void loop() {
    static bool overrideProcessed = false; // Flag to indicate if override has been processed

  if (mySerial.available() > 0) {
    String msg = mySerial.readStringUntil('\n');
    // Serial.println(overrideProcessed);
    
    if (!overrideProcessed) {
    
      if (msg.startsWith("+CMT:")) {
        Serial.println("------------inside trial block-----------");
  int space1 = msg.indexOf(' ');
  int space2 = msg.indexOf(',', space1 + 1);
  if (space1 != -1 && space2 != -1) {
    // Extract data
     c_no = msg.substring(space1 + 1, space2);
    Serial.println(c_no);
  }
      }
      if (msg.startsWith("getstatus")) {

        statSMS(c_no);
      }
      if (msg.startsWith("override")) {
        Wire.endTransmission(true);
        overrideProcessed = true;
        overrideReceived = true;
        sendSMS("Override received. Send data in the format:store <space> Name <space> Blood_group <space> Contact_no",c_no);
      }
    } else {
      // This block will execute only after override is received and processed
      if (msg.startsWith("store")) {
        
      if (overrideReceived) {
        Serial.println("condition entered");
        parseSMS(msg);
        overrideReceived = false; // Reset overrideReceived after parsing
        overrideProcessed = false; // Reset overrideProcessed to allow processing of next override
      }
      }
    }
    Serial.println(msg); // Print received SMS to Serial Monitor
  }
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      
  if (millis() - lastMillis >= interval) {
    lastMillis = millis();
    getCoordinates();
  }
    }
  }

  mpu_read();
  ax = (AcX - 2050) / 16384.00;
  ay = (AcY - 77) / 16384.00;
  az = (AcZ - 1947) / 16384.00;
  gx = (GyX + 270) / 131.07;
  gy = (GyY - 351) / 131.07;
  gz = (GyZ + 136) / 131.07;

  // calculating Amplitude vector for 3 axis
  float Raw_Amp = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
  int Amp = Raw_Amp * 10;  // Multiplied by 10 because values are between 0 to 1
  // Serial.println(Amp);

  if (Amp <= 2 && !trigger2) { // if AM breaks lower threshold (0.4g)
    trigger1 = true;
    Serial.println("TRIGGER 1 ACTIVATED");
  }
  
  if (trigger1) {
    trigger1count++;
    if (Amp >= 12) { // if AM breaks upper threshold (3g)
      trigger2 = true;
      Serial.println("TRIGGER 2 ACTIVATED");
      trigger1 = false;
      trigger1count = 0;
    }
  }
  
  if (trigger2) {
    trigger2count++;
    angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
    // Serial.println(angleChange);
    if (angleChange >= 30 && angleChange <= 400) { // if orientation changes by between 80-100 degrees
      trigger3 = true;
      trigger2 = false;
      trigger2count = 0;
      // Serial.println(angleChange);
      Serial.println("TRIGGER 3 ACTIVATED");
    }
  }
  
  if (trigger3) {
    trigger3count++;
    if (trigger3count >= 10) {
      angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
      // Serial.println(angleChange);
      if (angleChange >= 0 && angleChange <= 10) { // if orientation changes remains between 0-10 degrees
        fall = true;
        trigger3 = false;
        trigger3count = 0;
        // Serial.println(angleChange);
      }
      else { // user regained normal orientation
        trigger3 = false;
        trigger3count = 0;
        Serial.println("TRIGGER 3 DEACTIVATED");
      }
    }
  }
  
  if (Amp > 10) {
    crashed();
  }
  
  if (fall) { // in event of a fall detection
    Serial.println("FALL DETECTED");
    fall = false;
  }
  
  if (trigger2count >= 6) { // allow 0.5s for orientation change
    trigger2 = false;
    trigger2count = 0;
    Serial.println("TRIGGER 2 DECACTIVATED");
  }
  
  if (trigger1count >= 6) { // allow 0.5s for AM to break upper threshold
    trigger1 = false;
    trigger1count = 0;
    Serial.println("TRIGGER 1 DECACTIVATED");
  }
  
  delay(100);
}

void mpu_read() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);  // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}
void crashcall(){
//enter the send code for ambulance
    SOSSMS(c_no);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("wait for help!!!");
    lcd.setCursor(0, 1);
    lcd.print("SOS Alert");
    lcd.blink();
    Serial.println("-----------Crash SOS Call----------");
    Serial.println("Coordinates: ");
    Serial.print(gps.location.lat(), 8);
    Serial.print(" ");
    Serial.println(gps.location.lng(), 8);
    digitalWrite(Buzzerpin, LOW);
    delay(300000);
    normalmode();
    lcd.noBlink();
    Serial.print("crashed mode ----> normal mode");
}
void getCoordinates() {
  if (gps.location.isValid()) {
    // Serial.print("Latitude: ");
    // Serial.print(gps.location.lat(), 8);
    // Serial.print(" Longitude: ");
    // Serial.println(gps.location.lng(), 8);

    // Generate Google Maps link
    String mapLink = "https://www.google.com/maps/place/" + String(gps.location.lat(), 8) + "," + String(gps.location.lng(), 8);
    
    // Serial.print("Google Maps Link: ");
    // Serial.println(mapLink);


  } else {
    // Serial.println("No GPS data available");
  }
}
void sendSMS(const char* message,String phno) {
  mySerial.println("AT+CMGF=1");    // Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1 second
  
  // Replace the destination phone number with the appropriate one
  // String send_inst = 
  mySerial.println("AT+CMGS="+phno+"\r"); // Replace x with destination mobile number
  delay(1000);
  
  // Send the message
  mySerial.println(message);
  delay(100);
  
  mySerial.println((char)26); // ASCII code of CTRL+Z for ending the SMS
  delay(1000);
  Serial.println("exiting sendSMS!!!");
}
void sendSMSamb(const char* message) {
  mySerial.println("AT+CMGF=1");    // Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1 second
  
  // Replace the destination phone number with the appropriate one
  // String send_inst = 
  mySerial.println("AT+CMGS=\"+918217811621\"\r"); // Replace x with destination mobile number
  delay(1000);
  
  // Send the message
  mySerial.println(message);
  delay(100);
  
  mySerial.println((char)26); // ASCII code of CTRL+Z for ending the SMS
  delay(1000);
  Serial.println("exiting sendSMS!!!");
}

void receiveSMS() {
  Serial.println("in text mode");
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
}

void makeCall() {
  mySerial.println("ATD+918217811621;"); // Replace x with the phone number you want to call
  delay(1000); // Delay to allow time for the call to be initiated
}
void parseSMS(String msg) {
  // Split the received message into parts
  Serial.println("entered parsing");
  Serial.println(msg);
  int space1 = msg.indexOf(' ');
  int space2 = msg.indexOf(' ', space1 + 1);
  int space3 = msg.indexOf(' ', space2 + 1);
  if (space1 != -1 && space2 != -1 && space3 !=-1) {
    // Extract data
    command = msg.substring(0, space1);
    name = msg.substring(space1 + 1, space2);
    blood_group = msg.substring(space2 + 1,space3);
    contact_no = msg.substring(space3 + 1);
    // Reset override flag
    overrideReceived = false;
    Serial.println("exited Parsing");
    
    // Concatenate strings
    String temp_stat = "successfully added data: \n";
    temp_stat.concat("Name: ");
    temp_stat.concat(name);
    temp_stat.concat("\nBlood_type: ");
    temp_stat.concat(blood_group);
    temp_stat.concat("\nContact: ");
    temp_stat.concat(contact_no);
    Serial.println("---------final output---------");
    Serial.println(temp_stat);
    sendSMS(temp_stat.c_str(),c_no); // Convert String to const char* and pass to sendSMS
  } 
  normalmode();
}
void statSMS(String phno){
  String temp_stat = "current data: \n";
    temp_stat.concat("Name: ");
    temp_stat.concat(name);
    temp_stat.concat("\nBlood_type: ");
    temp_stat.concat(blood_group);
    temp_stat.concat("\nContact: ");
    temp_stat.concat(contact_no);
    Serial.println("---------final output---------");
    Serial.println(temp_stat);
    sendSMS(temp_stat.c_str(),phno);
}
void SOSSMS(String phno){
      String mapLink = "https://www.google.com/maps/place/" + String(gps.location.lat(), 8) + "," + String(gps.location.lng(), 8);
  String SOS_stat = "SOS data: \n";
    SOS_stat.concat("Name: ");
    SOS_stat.concat(name);
    SOS_stat.concat("\nBlood_type: ");
    SOS_stat.concat(blood_group);
    SOS_stat.concat("\nContact: ");
    SOS_stat.concat(contact_no);
    // SOS_stat.concat("\nCoordinates: ");
    // SOS_stat.concat(gps.location.lat(), 8);
    // SOS_stat.concat(" ");
    // SOS_stat.concat(gps.location.lng(), 8);
    SOS_stat.concat("\nMap link: ");
    SOS_stat.concat(mapLink);
    Serial.println("---------final output---------");
    Serial.println(SOS_stat);
    sendSMSamb(SOS_stat.c_str());
}