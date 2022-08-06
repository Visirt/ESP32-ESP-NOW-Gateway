#include <espnow.h>
#include <mqtt.h>
#include <constants_espnow.h>
#include <EspNowFloodingMesh.h>

unsigned char secredKey[] = SECRET_KEY;

void ESPNOW::setupEspNow()
{
  espNowFloodingMesh_RecvCB(onRecv);
  espNowFloodingMesh_secredkey(secredKey);
  espNowFloodingMesh_begin(CHANNEL, BSID, "");
  espNowFloodingMesh_setToMasterRole(true, 3);

  #ifdef DEBUG
    Serial.println("ESP-NOW SETUP");
  #endif
}

String IntToString(uint64_t node)
{
  uint64_t aux = node;
  String res = "";
  while (aux > 0)
  {
    res = ((char) (aux % 256)) + res;
    aux >>= 8;
  }
  return res;
}

void ESPNOW::onRecv(const uint8_t *data, int len, uint64_t from, uint32_t replyPrt) {
  String str = String((char*)data).substring(0, len);
  #ifdef DEBUG
    Serial.println(str);
  #endif
    
    if(from != 0)
    {
      String sendTopic = "esp32Mesh/from/" + IntToString(from);
      MQTT::mqttClient.publish(sendTopic.c_str(), 2, false, str.c_str());
      return;
    }
  MQTT::mqttClient.publish(("esp32Mesh/invalid/" + IntToString(from)).c_str(), 2, false, "");
}

esp_err_t ESPNOW::sendData(String message, const String dest){
  #ifdef DEBUG
    Serial.println("Sending Data:" + message);
  #endif
  return espNowFloodingMesh_sendWithACK((uint8_t*)message.c_str(), message.length(), dest);
}