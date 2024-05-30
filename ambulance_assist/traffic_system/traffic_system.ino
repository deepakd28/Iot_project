#include <esp_now.h>
#include <WiFi.h>

const int red1 = 2;     // Red light for lane 1
const int yellow1 = 4;  // Yellow light for lane 1
const int green1 = 16;   // Green light for lane 1


const int red2 = 19;    // Red light for lane 2
const int yellow2 = 18; // Yellow light for lane 2
const int green2 = 21;  // Green light for lane 2


const int red3 = 14;    // Red light for lane 3
const int yellow3 = 13; // Yellow light for lane 3
const int green3 = 12;  // Green light for lane 3


const int red4 = 32;    // Red light for lane 4
const int yellow4 = 33; // Yellow light for lane 4
const int green4 = 25;  // Green light for lane 4
  int pre_data;
  int data;
  int Lastlane=0;
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data_recv, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Lane no: "); Serial.println(*data_recv);
  Serial.println("");
  data= *data_recv;
  
}


void setup() {
  data=0;
  pre_data=0;
  pinMode(red1, OUTPUT);
  pinMode(yellow1, OUTPUT);
  pinMode(green1, OUTPUT);


  pinMode(red2, OUTPUT);
  pinMode(yellow2, OUTPUT);
  pinMode(green2, OUTPUT);


  pinMode(red3, OUTPUT);
  pinMode(yellow3, OUTPUT);
  pinMode(green3, OUTPUT);


  pinMode(red4, OUTPUT);
  pinMode(yellow4, OUTPUT);
  pinMode(green4, OUTPUT);


  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);



}

void loop() {
 if(data==0&&pre_data==0){
  Lane1Normal();
  }else if(data==1){
  lane1Sequence();
 }else if(data==2){
  lane2Sequence();
 }else if(data==3){
  lane3Sequence();
 }else if(data==4){
  lane4Sequence();
 }
  if(data==0&&pre_data==0){
  Lane2Normal();
  }else if(data==2){
  lane2Sequence();
 }else if(data==3){
  lane3Sequence();
 }else if(data==4){
  lane4Sequence();
 }
 if(data==0&&pre_data==0){
  Lane3Normal();
  }else if(data==2){
  lane2Sequence();
 }else if(data==3){
  lane3Sequence();
 }else if(data==4){
  lane4Sequence();
 }
 if(data==0&&pre_data==0){
  Lane4Normal();
  }else if(data==2){
  lane2Sequence();
 }else if(data==3){
  lane3Sequence();
 }else if(data==4){
  lane4Sequence();
 }
}

 // Lane 1
 void Lane1Normal(){
    Serial.println("in normal sequence.");
    Serial.print("data:");
    Serial.println(data);
    Serial.print("pre_data:");
    Serial.println(pre_data);

    digitalWrite(red2, HIGH);
    digitalWrite(red3, HIGH);
    digitalWrite(red4, HIGH);

    digitalWrite(yellow1, HIGH);
    delay(1000);
    digitalWrite(red1, HIGH);
    delay(1000);
    digitalWrite(red1, LOW);
    digitalWrite(yellow1, LOW);
    digitalWrite(green1, HIGH);
    delay(5000);
    digitalWrite(green1, LOW);
    Lastlane=1;
 }
  // Transition delay

  // Lane 2
  void Lane2Normal(){
    digitalWrite(red1, HIGH);
    digitalWrite(red3, HIGH);
    digitalWrite(red4, HIGH);

    digitalWrite(yellow2, HIGH);
    delay(1000);
    digitalWrite(red2, HIGH);
    delay(1000);
    digitalWrite(red2, LOW);
    digitalWrite(yellow2, LOW);
    digitalWrite(green2, HIGH);
    delay(5000);
    digitalWrite(green2, LOW);
    Lastlane=2;
  }
  // Transition delay

  // Lane 3
  void Lane3Normal(){
    digitalWrite(red1, HIGH);
    digitalWrite(red2, HIGH);
    digitalWrite(red4, HIGH);

    digitalWrite(yellow3, HIGH);
    delay(1000);
    digitalWrite(red3, HIGH);
    delay(1000);
    digitalWrite(red3, LOW);
    digitalWrite(yellow3, LOW);
    digitalWrite(green3, HIGH);
    delay(5000);
    digitalWrite(green3, LOW);
    Lastlane=3;
  }
  // Transition delay

  // Lane 4
  void Lane4Normal(){
    digitalWrite(red1, HIGH);
    digitalWrite(red2, HIGH);
    digitalWrite(red3, HIGH);

    digitalWrite(yellow4, HIGH);
    delay(1000);
    digitalWrite(red4, HIGH);
    delay(1000);
    digitalWrite(red4, LOW);
    digitalWrite(yellow4, LOW);
    digitalWrite(green4, HIGH);
    delay(5000);
    digitalWrite(green4, LOW);
    Lastlane=4;
}

void  lane1Sequence(){
  if(data==1&&pre_data==0){
    Serial.print("lane 1 sequence override sequence:\n\n");
  digitalWrite(green1, HIGH);
  digitalWrite(red1, LOW);
  digitalWrite(yellow1, LOW);

  digitalWrite(red2,HIGH);
  digitalWrite(green2,LOW);
  digitalWrite(yellow2,LOW);

  digitalWrite(red3,HIGH);
  digitalWrite(green3,LOW);
  digitalWrite(yellow3,LOW);

  digitalWrite(red4,HIGH);
  digitalWrite(green4,LOW);
  digitalWrite(yellow4,LOW);
  pre_data=1;
  data=0;
  return;
  }else if(data!=0&&pre_data!=0){
    pre_data=0;
    data=0;
    Serial.print("lane 1 sequence\n back to normal\n\n");
    digitalWrite(green1,LOW);
    digitalWrite(green2,LOW);
    digitalWrite(green3,LOW);
    digitalWrite(green4,LOW);
    switch(Lastlane){
      case 1:Lane1Normal();
      break;
      case 2:Lane2Normal();
      break;
      case 3:Lane3Normal();
      break;
      case 4:Lane4Normal();
      break;
    }
  }
}

void  lane2Sequence(){
  if(data==2&&pre_data==0){
    Serial.print("lane 2 Override sequence:\n\n");
  digitalWrite(red1, HIGH);
  digitalWrite(green1, LOW);
  digitalWrite(yellow1, LOW);

  digitalWrite(green2 ,HIGH);
  digitalWrite(red2, LOW);
  digitalWrite(yellow2,  LOW);

  digitalWrite(red3, HIGH);
  digitalWrite(green3, LOW);
  digitalWrite(yellow3, LOW);

  digitalWrite(red4, HIGH);
  digitalWrite(green4, LOW);
  digitalWrite(yellow4, LOW);
  data=0;
  pre_data=2;
  return;
  }else if(data!=0&&pre_data!=0) {
    data=0;
    pre_data=0;
    Serial.print("lane 2 sequence \n back to normal \n\n");
    digitalWrite(green1,LOW);
    digitalWrite(green2,LOW);
    digitalWrite(green3,LOW);
    digitalWrite(green4,LOW);
    switch(Lastlane){
      case 1:Lane1Normal();
      break;
      case 2:Lane2Normal();
      break;
      case 3:Lane3Normal();
      break;
      case 4:Lane4Normal();
      break;
    }
  }
}

void  lane3Sequence(){
  if(data==3&&pre_data==0){
   Serial.print("lane 3 Override sequence:\n\n");
  digitalWrite(red1, HIGH);
  digitalWrite(green1, LOW);
  digitalWrite(yellow1, LOW);

  digitalWrite(red2, HIGH);
  digitalWrite(green2, LOW);
  digitalWrite(yellow2, LOW);

  digitalWrite(green3, HIGH); 
  digitalWrite(red3, LOW);
  digitalWrite(yellow3, LOW);

  digitalWrite(red4, HIGH);
  digitalWrite(green4, LOW);
  digitalWrite(yellow4, LOW);
  pre_data=3;
  data=0;
  }else if(data!=0&&pre_data!=0){
    data=0;
    pre_data=0;
    Serial.print("lane 3 sequence \n back to normal \n\n");
    digitalWrite(green1,LOW);
    digitalWrite(green2,LOW);
    digitalWrite(green3,LOW);
    digitalWrite(green4,LOW);
    switch(Lastlane){
      case 1:Lane1Normal();
      break;
      case 2:Lane2Normal();
      break;
      case 3:Lane3Normal();
      break;
      case 4:Lane4Normal();
      break;
    }
  }
}

void  lane4Sequence(){
  if(data==4&&pre_data==0){
  Serial.print("lane 4 Override sequence:\n\n");
  digitalWrite(red1,HIGH);
  digitalWrite(green1,LOW);
  digitalWrite(yellow1,LOW);

  digitalWrite(red2,HIGH);
  digitalWrite(green2,LOW);
  digitalWrite(yellow2,LOW);

  digitalWrite(red3,HIGH);
  digitalWrite(green3,LOW);
  digitalWrite(yellow3,LOW);

  digitalWrite(green4,HIGH);
  digitalWrite(red4,LOW);
  digitalWrite(yellow4,LOW);
  pre_data=4;
  data=0;
  }else if(data!=0&&pre_data!=0){
    data=0;
    pre_data=0;
    Serial.print("lane 4 sequence \n back to normal \n\n");
    digitalWrite(green1,LOW);
    digitalWrite(green2,LOW);
    digitalWrite(green3,LOW);
    digitalWrite(green4,LOW);
    switch(Lastlane){
      case 1:Lane1Normal();
      break;
      case 2:Lane2Normal();
      break;
      case 3:Lane3Normal();
      break;
      case 4:Lane4Normal();
      break;
    }
  }
}
