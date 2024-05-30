#include <HardwareSerial.h>
#include <EEPROM.h>

HardwareSerial sim800L(2); 

const int switch1 = 15;
const int switch2 = 4;
const int on_switch = 23;
const int off_switch = 22;
const int dry_pin = 19;
const int current_pin = 18;
int master_address = 0;
int slave1_address = 21;
int slave2_address = 42;
int slave3_address = 63;
int slave4_address = 84;
int autostart_add = 105;
bool autostart;
bool autostat = false;
int dry_run = 0;
int flag=0;
String buff;
String buff2;
String buff3;
String c_no;
String stored_no;
String temp_no;
String s_no;
String master_no;
String slave1_no;
String slave2_no;
String slave3_no;
String slave4_no;
String contact_list;
String motor_stat;


void setup() {
  pinMode(current_pin, INPUT);
  pinMode(on_switch, INPUT_PULLUP);
  pinMode(off_switch, INPUT_PULLUP);
  pinMode(switch1, OUTPUT);
  pinMode(switch2, OUTPUT);
  digitalWrite(switch1, HIGH); 
  digitalWrite(switch2, HIGH);
  Serial.begin(9600);
  Serial.println("\nTesting EEPROM Library\n");
    if (!EEPROM.begin(1000)) {
      Serial.println("Failed to initialise EEPROM");
      Serial.println("Restarting...");
      delay(1000);
      ESP.restart();
    }
  sim800L.begin(9600);
  Serial.println("Initializing...");
  sim800L.println("AT");
  waitForResponse();
  // sim800L.println("AT+CMGF=1");
  // waitForResponse();
  // sim800L.println("AT+CNMI=1,2,0,0,0");
  // waitForResponse();
  // sim800L.println("AT+CMGDA=\"DEL ALL\"");
  // waitForResponse();
  // sim800L.println("AT+CCLK?");
  // waitForResponse();
  // sim800L.println("AT+CLTS=1");
  // waitForResponse();
  // sim800L.println("AT&w");
  // waitForResponse();
  master_no=EEPROM.readString(master_address);
  slave1_no=EEPROM.readString(slave1_address);
  slave2_no=EEPROM.readString(slave2_address);
  slave3_no=EEPROM.readString(slave3_address);
  slave4_no=EEPROM.readString(slave4_address);
  autostart=EEPROM.readBool(autostart_add);
}


void loop() {
  // get_time();
  // delay(10000);
  int onbtn_stat = digitalRead(on_switch);
  if(onbtn_stat==LOW){
    Serial.println(onbtn_stat);
    pumpon(0);
  }
  int offbtn_stat = digitalRead(off_switch);
  if(offbtn_stat==LOW){
    pumpoff(0);
    delay(2000);
  }
  int current_stat = digitalRead(current_pin);
  if(current_stat==1 && flag == 0){
    Serial.println("-------------current on--------------");
    flag=1;
    if(autostart == true){
      autostat=true;
      delay(5000);
      pumpon(1);
    }else if(autostart == false){
      send_sms("Current : ON",master_no);
      delay(2500);
      send_sms("Current : ON",slave1_no);
      delay(2500);
      send_sms("Current : ON",slave2_no);
      delay(2500);
      send_sms("Current : ON",slave3_no);
      delay(2500);
      send_sms("Current : ON",slave4_no);
      delay(2500);
    Serial.println("-------------All sms sent--------------");
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
    }
  }
  if(current_stat==0 && flag == 1){
    flag=0;
    motor_stat="OFF";
      Serial.println("-------------current off--------------");
      send_sms("Current : OFF",master_no);
      delay(2500);
      send_sms("Current : OFF",slave1_no);
      delay(2500);
      send_sms("Current : OFF",slave2_no);
      delay(2500);
      send_sms("Current : OFF",slave3_no);
      delay(2500);
      send_sms("Current : OFF",slave4_no);
      delay(2500);
    Serial.println("-------------All sms sent--------------");
  }
  while(sim800L.available()){
    buff = sim800L.readStringUntil('\n');
    delay(1000);
    buff2 = sim800L.readStringUntil('\n');
    delay(1000);
    buff3 = sim800L.readStringUntil('\n');
    Serial.println("------------start recieved message 1 -----------");
    Serial.println(buff);
    Serial.println("------------end recieved message 1 -----------");
    Serial.println("------------start recieved message 2 -----------");
    Serial.println(buff2);
    Serial.println("------------end recieved message 2 -----------");
    Serial.println("------------start recieved message 3 -----------");
    Serial.println(buff3);
    Serial.println("------------end recieved message 3 -----------");
    if (buff2.startsWith("+CMT:")) {
      Serial.println("------------inside current no prase block-----------");
      int space1 = buff2.indexOf('"');
      int space2 = buff2.indexOf('"', space1 + 1);
      if (space1 != -1 && space2 != -1) {
      c_no = buff2.substring(space1 + 1, space2);
      Serial.println(c_no);
      Serial.println(buff2);
      buff2 = "\n";
      }
    }

    
    if(isValidPhone(c_no)){
      if (buff3.startsWith("onmotor")) {
      pumpon(1);
    }
    if (buff3.startsWith("offmotor")) {
      pumpoff(1);
    }
    if (buff3.startsWith("onautostart")) {
      Serial.println(buff);
      EEPROM.writeBool(autostart_add, true);
      EEPROM.commit();
      send_sms("Autostarter Status: ON",c_no);
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
      update_info();
    }
    if (buff3.startsWith("offautostart")) {
      Serial.println(buff);
      EEPROM.writeBool(autostart_add, false);
      EEPROM.commit();
      send_sms("Autostarter Status: OFF",c_no);
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
      update_info();
    }
    if (buff3.startsWith("clist")) {
      update_info();
      send_sms(clist(),c_no);
    }
    }else{
      send_sms("Access Denied!!!!\nRestricted access contact the Owner.",c_no);
      c_no="\n";
    }
    if (buff3.startsWith("addmaster")) {
      Serial.println(buff);
      char master_numb[16];
      int len = c_no.length();
      len = (len < 16) ? len : 15; // Ensure we don't exceed the array bounds
      c_no.toCharArray(master_numb, len + 1); // Convert String to char array
      Serial.println("--------start------Resulting number:---------------");
      Serial.println("Resulting number:");
      Serial.println(master_numb);
      Serial.println("--------end------Resulting number:---------------");
      EEPROM.writeString(master_address, master_numb);
      EEPROM.commit();
      Serial.println("--------start------stored number:---------------");
      stored_no=EEPROM.readString(master_address);
      Serial.println(EEPROM.readString(master_address));
      Serial.println("--------end------stored number:---------------");
      send_sms("Master added successfully!!",stored_no);
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
      update_info();
    }
    if (buff3.startsWith("addslave1")) {
      if(isMaster(c_no)){
      temp_no=phraseslave(buff3);
      char slave1_numb[16];
      int len = temp_no.length();
      len = (len < 16) ? len : 15; // Ensure we don't exceed the array bounds
      temp_no.toCharArray(slave1_numb, len + 1); // Convert String to char array
      Serial.println("--------start------Resulting number:---------------");
      Serial.println("Resulting number:");
      Serial.println(slave1_numb);
      Serial.println("--------end------Resulting number:---------------");
      EEPROM.writeString(slave1_address, slave1_numb);
      EEPROM.commit();
      Serial.println("--------start------stored number:---------------");
      stored_no=EEPROM.readString(slave1_address);
      Serial.println(EEPROM.readString(slave1_address));
      Serial.println("--------end------stored number:---------------");
      send_sms("Slave added successfully!!",c_no);
      delay(2000);
      send_sms("Number added successfully!!",stored_no);
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
      update_info();
      }else{
      send_sms("Access Denied!!!!\nRestricted access contact the Owner.",c_no);
    }
    }
    if (buff3.startsWith("addslave2")) {
      if(isMaster(c_no)){
      temp_no=phraseslave(buff3);
      char slave2_numb[16];
      int len = temp_no.length();
      len = (len < 16) ? len : 15; // Ensure we don't exceed the array bounds
      temp_no.toCharArray(slave2_numb, len + 1); // Convert String to char array
      Serial.println("--------start------Resulting number:---------------");
      Serial.println("Resulting number:");
      Serial.println(slave2_numb);
      Serial.println("--------end------Resulting number:---------------");
      EEPROM.writeString(slave2_address, slave2_numb);
      EEPROM.commit();
      Serial.println("--------start------stored number:---------------");
      stored_no=EEPROM.readString(slave2_address);
      Serial.println(EEPROM.readString(slave2_address));
      Serial.println("--------end------stored number:---------------");
      send_sms("Slave added successfully!!",c_no);
      delay(2000);
      send_sms("Number added successfully!!",stored_no);
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
      update_info();
      }else{
      send_sms("Access Denied!!!!\nRestricted access contact the Owner.",c_no);
    }
    }
    if (buff3.startsWith("addslave3")) {
     if(isMaster(c_no)){
      temp_no=phraseslave(buff3);
      char slave3_numb[16];
      int len = temp_no.length();
      len = (len < 16) ? len : 15; // Ensure we don't exceed the array bounds
      temp_no.toCharArray(slave3_numb, len + 1); // Convert String to char array
      Serial.println("--------start------Resulting number:---------------");
      Serial.println("Resulting number:");
      Serial.println(slave3_numb);
      Serial.println("--------end------Resulting number:---------------");
      EEPROM.writeString(slave3_address, slave3_numb);
      EEPROM.commit();
      Serial.println("--------start------stored number:---------------");
      stored_no=EEPROM.readString(slave3_address);
      Serial.println(EEPROM.readString(slave3_address));
      Serial.println("--------end------stored number:---------------");
      send_sms("Slave added successfully!!",c_no);
      delay(2000);
      send_sms("Number added successfully!!",stored_no);
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
      update_info();
      }else{
      send_sms("Access Denied!!!!\nRestricted access contact the Owner.",c_no);
    }
    }
    if (buff3.startsWith("addslave4")) {
      if(isMaster(c_no)){
      temp_no=phraseslave(buff3);
      char slave4_numb[16];
      int len = temp_no.length();
      len = (len < 16) ? len : 15; // Ensure we don't exceed the array bounds
      temp_no.toCharArray(slave4_numb, len + 1); // Convert String to char array
      Serial.println("--------start------Resulting number:---------------");
      Serial.println("Resulting number:");
      Serial.println(slave4_numb);
      Serial.println("--------end------Resulting number:---------------");
      EEPROM.writeString(slave4_address, slave4_numb);
      EEPROM.commit();
      Serial.println("--------start------stored number:---------------");
      stored_no=EEPROM.readString(slave4_address);
      Serial.println(EEPROM.readString(slave4_address));
      Serial.println("--------end------stored number:---------------");
      send_sms("Slave added successfully!!",c_no);
      delay(2000);
      send_sms("Number added successfully!!",stored_no);
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
      update_info();
    }else{
      send_sms("Access Denied!!!!\nRestricted access contact the Owner.",c_no);
    }
    }
    if (buff3.startsWith("del")) {
     if(isMaster(c_no)){
      int switch_index = phraseslave(buff3).toInt();
      switch (switch_index) {
      case 1:
          del_no(slave1_address, slave1_address + 20);
          break;
        case 2:
          del_no(slave2_address, slave2_address + 20);
          break;
        case 3:
          del_no(slave3_address, slave3_address + 20);
          break;
        case 4:
          del_no(slave4_address, slave4_address + 20);
          break;
        default:
          send_sms("Invalid Choice!!!!!", c_no);
      }
    } else {
      send_sms("Access Denied!!!!\nRestricted access contact the Owner.", c_no);
    
    }
}
 if (buff3.startsWith("del master")) {
  if(isMaster(c_no)){
    del_no(master_address, master_address + 20);
  }else {
      send_sms("Access Denied!!!!\nRestricted access contact the Owner.", c_no);
    }
 }
    if (buff3.startsWith("del_override")) {
      Del_all();
    }
  }
}

void send_sms(String msg, String phone) {
    if (phone.length() > 0 && phone != "\n") {
        sim800L.print("AT+CMGS=\"");
        sim800L.print(phone);
        sim800L.print("\"\r");
        waitForResponse();
        sim800L.print(msg);
        sim800L.write(0x1A); // Send Ctrl+Z to indicate end of message
        waitForResponse();
    } else {
        Serial.println("Number does not exist!!!!");
        return;
    }
}


void waitForResponse(){
  delay(1000);
  while(sim800L.available()){
    Serial.println(sim800L.readString());
  }
  sim800L.read();
}

void pumpon(int ch){
  if(ch){
  digitalWrite(switch1, LOW);
  delay(1000);                      // wait for a second
  digitalWrite(switch1, HIGH);
  }
  Serial.println("--------------motor on---------------"); 
  motor_stat="ON";
  if(autostat == true){
    autostat=false;
      delay(5000);
      send_sms(update_sms(),master_no);
      delay(1000);
      send_sms(update_sms(),slave1_no);
      delay(1000);
      send_sms(update_sms(),slave2_no);
      delay(1000);
      send_sms(update_sms(),slave3_no);
      delay(1000);
      send_sms(update_sms(),slave4_no);
      delay(1000);
    Serial.println("-------------All sms sent--------------");
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
  }else if(autostat == false){
      send_sms(update_sms(),master_no);
      delay(1000);
      send_sms(update_sms(),slave1_no);
      delay(1000);
      send_sms(update_sms(),slave2_no);
      delay(1000);
      send_sms(update_sms(),slave3_no);
      delay(1000);
      send_sms(update_sms(),slave4_no);
      delay(1000);
      Serial.println("-------------All sms sent--------------");
      sim800L.println("AT+CMGDA=\"DEL ALL\"");
      waitForResponse();
    }
    dry_test();
}

void pumpoff(int ch){
  if(ch){
  digitalWrite(switch2, LOW);
  delay(1000);
  digitalWrite(switch2, HIGH);
  }
  Serial.println("----------motor off-------------");
  motor_stat="OFF";
  send_sms(update_sms(),master_no);
  delay(1000);
  send_sms(update_sms(),slave1_no);
  delay(1000);
  send_sms(update_sms(),slave2_no);
  delay(1000);
  send_sms(update_sms(),slave3_no);
  delay(1000);
  send_sms(update_sms(),slave4_no);
  delay(1000);
  Serial.println("-------------All sms sent--------------");
  sim800L.println("AT+CMGDA=\"DEL ALL\"");
  waitForResponse(); 
}
void dry_test(){
  delay(5000);
  int dry_run = digitalRead(dry_pin);
  if(dry_run == 0){
    digitalWrite(switch2, LOW);
    delay(1000);                   
    digitalWrite(switch2, HIGH); 
    Serial.println("Dry run : Failed Moter off");
    send_sms("Dry run : Failed\nMoter off",master_no);
    delay(1000);
    send_sms("Dry run : Failed\nMoter off",slave1_no);
    delay(1000);
    send_sms("Dry run : Failed\nMoter off",slave2_no);
    delay(1000);
    send_sms("Dry run : Failed\nMoter off",slave3_no);
    delay(1000);
    send_sms("Dry run : Failed\nMoter off",slave4_no);
    delay(1000);
    Serial.println("-------------All sms sent--------------");
    sim800L.println("AT+CMGDA=\"DEL ALL\"");
    waitForResponse();
  }

}

String phraseslave(String msg){
  if (buff3.startsWith("add")) {
    Serial.println("------------inside slave no prase block-----------");
    int space3 = buff3.indexOf('(');
    int space4 = buff3.indexOf(')', space3+1);
    if (space3 != -1 && space4 != -1) {
      s_no = buff3.substring(space3 + 1, space4);
      Serial.println("=================start=================");
      Serial.println(s_no);
      Serial.println("====================end==============");
    return(s_no);
    }
  }
  if (buff3.startsWith("del")) {
    int space3 = buff3.indexOf('(');
    int space4 = buff3.indexOf(')', space3+1);
    if (space3 != -1 && space4 != -1) {
      String index = buff3.substring(space3 + 1, space4);
      Serial.println("=================start=================");
      Serial.println(index);
      Serial.println("====================end==============");
      return(index);
    }
  }
}
void del_no(int start, int end) {
  if (start < 0 || end >= EEPROM.length()) {
    // Out of bounds check
    Serial.println("Invalid EEPROM address range.");
    return;
  }

  for (int i = start; i <= end; i++) {
    EEPROM.write(i, 0); // Write 0 to each byte in EEPROM
  }

  bool commitSuccess = EEPROM.commit(); // Commit changes to EEPROM
  if (!commitSuccess) {
    Serial.println("EEPROM commit failed");
    send_sms("Delete was not Successful!!!",c_no);
  } else {
    Serial.println("EEPROM commit successful");
    send_sms("Delete was Successful!!!",c_no);
    delay(1000);
    send_sms(clist(),c_no);
  }
}
void update_info(){
  master_no=EEPROM.readString(master_address);
  slave1_no=EEPROM.readString(slave1_address);
  slave2_no=EEPROM.readString(slave2_address);
  slave3_no=EEPROM.readString(slave3_address);
  slave4_no=EEPROM.readString(slave4_address);
  autostart=EEPROM.readBool(autostart_add);
}
bool isValidPhone(String phone) {
  if (master_no.equals(phone) || master_no.equals("\n")) {
    return true;
  }else if (slave1_no.equals(phone)) {
    return true;
  }else if (slave2_no.equals(phone)) {
    return true;
  }else if (slave3_no.equals(phone)) {
    return true;
  }else if (slave4_no.equals(phone)) {
    return true;
  }else{
    return false; 
  }
}
bool isMaster(String phone) {
  if (master_no.equals(phone)) {
    return true;
  }else{
    return false; 
  }
}
void Del_all() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0); // Write 0 to each byte in EEPROM
  }
  EEPROM.commit();
  delay(1000);
  send_sms(clist(),c_no); // Commit changes to EEPROM
}
String clist(){
  String sms ="Contact List:\n1: "+master_no+"\n2: "+slave1_no+"\n3: "+slave2_no+"\n4: "+slave3_no+"\n5:"+slave4_no;
  return(sms);
}

String update_sms(){
  String autostart_sms;
  String time= get_time();
  if(autostart){
    autostart_sms="ON";
  }else{
    autostart_sms="OFF";
  }
  String stat_sms="Motor Status :"+motor_stat+"\n"+"Autostarter Status :"+autostart_sms+"\n"+"Time :"+time;
  return stat_sms;
}

String get_time(){
  String time;
  sim800L.println("AT+CCLK?");
  delay(1000);
  while(sim800L.available()){
  time=sim800L.readString();
  }
  sim800L.readString();
   int space1 = time.indexOf('"');
      int space2 = time.indexOf('+', space1 + 1);
      if (space1 != -1 && space2 != -1) {
      time = time.substring(space1 + 1, space2);
      Serial.println("--------time---------");
      Serial.println(time);
      Serial.println("--------time---------");
  return time;
      }
}
