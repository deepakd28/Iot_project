#include <esp_now.h>
#include <WiFi.h>




uint8_t receiverAdd[] = {0x40, 0x22, 0xd8, 0x4f, 0x0e, 0x05};
esp_now_peer_info_t peerInfo;

int data=0;
String sta;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(status == ESP_NOW_SEND_SUCCESS) 
  sta="Delivery Success"; else sta="Delivery Fail";
 Serial.println(sta);
}

void setup() {
  Serial.begin(115200);

    
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, receiverAdd, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
 
}


void draw()
  {
  
  }

void loop() {
data++;
esp_now_send(receiverAdd, (uint8_t *) &data, sizeof(data));
Serial.print("data sent:");
Serial.println(data);
delay(2000);
}