#include <espnow.h>
#include <mqtt.h>
#include <ArduinoJson.h>

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
  if(!esp_now_is_peer_exist(mac_addr))
    addSlave(mac_addr);
  String data = String((char*)incomingData).substring(0, len);
  DynamicJsonDocument  doc(1024);
  if(ArduinoJson::deserializeJson(doc, data) == ArduinoJson6194_F1::DeserializationError::Ok)
  {
    JsonObject obj = doc.as<JsonObject>();

    String from = obj["from"];
    String relay = obj["relay"];
    String message = obj["message"];

    if(message != nullptr && !relay.isEmpty() && !from.isEmpty())
    {
      String sendTopic = "esp32Mesh/from/";
      if(from != relay)
        sendTopic += relay + "/";
      sendTopic += from;
      MQTT::mqttClient.publish(sendTopic.c_str(), 2, false, message.c_str());
      return;
    }
  }
  MQTT::mqttClient.publish(("esp32Mesh/invalid/" + macAddrString(mac_addr)).c_str(), 2, false, "");
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

uint8_t* ESPNOW::macAddrString(const String mac_addr)
{
  if(mac_addr.length() != 13)
    return nullptr;
  uint8_t* macaddr =  new uint8_t[6];
  for(int i = 0 ; i < 6 ; i++)
  {
    macaddr[i] = (uint8_t)std::strtoul(mac_addr.substring(4 * i, 4 * i + 3).c_str(), nullptr, 16);
  }
  return macaddr;
}

esp_err_t ESPNOW::addSlave(const uint8_t* const address){
  esp_now_peer_info_t slaveInfo;
  memcpy(slaveInfo.peer_addr, address, 6);
  slaveInfo.channel = 0;  
  slaveInfo.encrypt = false;
  
  return esp_now_add_peer(&slaveInfo);
}


esp_err_t ESPNOW::sendData(String message, const uint8_t* const dest, const uint8_t* const relay){
  if(!esp_now_is_peer_exist(relay))
    addSlave(relay);
  String sendingData = "{\"to\":\"" + macAddrString(dest) + "\",\"relay\":\"" + macAddrString(relay) + "\",\"message\":\"" + message + "\"}";
  #ifdef DEBUG
    Serial.println("Sending Data:" + sendingData);
  #endif
  return esp_now_send(relay, (uint8_t*)sendingData.c_str(), sendingData.length());
}