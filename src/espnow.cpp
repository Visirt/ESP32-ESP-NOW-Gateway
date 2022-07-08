#include <espnow.h>
#include <mqtt.h>

void ESPNOW::setupEspNow()
{
  if (esp_now_init() != ESP_OK) {
    #ifdef DEBUG
        Serial.println("There was an error initializing ESP-NOW");
    #endif
    delay(10000);
    ESP.restart();
    return;
  }
  else
  {
    #ifdef DEBUG
      Serial.println("ESP-NOW Initialized");
    #endif
  }
  esp_now_register_send_cb(onSent);
  esp_now_register_recv_cb(onRecv);
}

void ESPNOW::onRecv(const uint8_t* mac_addr, const uint8_t* incomingData, int len) {
  String data = String((char*)incomingData).substring(0, len);
  int semiColonLocation = data.indexOf(';');
  int numItems = data.substring(0, semiColonLocation).toInt();
  String topic = data.substring(semiColonLocation + 1, numItems + semiColonLocation + 1);
  if(topic.isEmpty())
    topic = "from";
  String message = data.substring(semiColonLocation + numItems + 1);
  MQTT::mqttClient.publish(("esp32Mesh/" + topic + "/" + macAddrString(mac_addr)).c_str(), 2, false, message.c_str());
}

void ESPNOW::onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(status != ESP_NOW_SEND_SUCCESS)
  {
    #ifdef DEBUG
        Serial.println("ERROR SENDING DATA");
    #endif
  }
}

String ESPNOW::macAddrString(const uint8_t * const mac_addr)
{
  return String(mac_addr[0], HEX) + ":" + String(mac_addr[1], HEX) + ":" + String(mac_addr[2], HEX) + ":" + String(mac_addr[3], HEX) + ":" + String(mac_addr[4], HEX) + ":" + String(mac_addr[5], HEX);
}

esp_err_t ESPNOW::addSlave(const uint8_t* const address){
  esp_now_peer_info_t slaveInfo;
  memcpy(slaveInfo.peer_addr, address, 6);
  slaveInfo.channel = 0;  
  slaveInfo.encrypt = false;
  
  return esp_now_add_peer(&slaveInfo);
}


esp_err_t ESPNOW::sendData(String message, String topic, uint8_t* dest){
  String sendingData = String(topic.length()) + ";" + topic + message;
  Serial.println("Sending Data:" + sendingData);
  return esp_now_send(dest, (uint8_t*)sendingData.c_str(), sendingData.length());
}