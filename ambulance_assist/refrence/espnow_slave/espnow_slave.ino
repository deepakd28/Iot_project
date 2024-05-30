#include <esp_now.h>
#include <WiFi.h>

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Lane no: "); Serial.println(*data);
  Serial.println("");
}

void setup() {
  Serial.begin(115200);
    int data=0;
  WiFi.mode(WIFI_AP);
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);
}



void loop() {

}
