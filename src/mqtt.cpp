#include <mqtt.h>
#include <constants_mqtt.h>
#include <WiFi.h>
#include <espnow.h>

AsyncMqttClient MQTT::mqttClient;

void MQTT::setupMqtt()
{
    mqttClient.setClientId(MQTT_NAME);
    mqttClient.setCredentials(MQTT_USER, MQTT_PASSWD);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.setKeepAlive(30);
    mqttClient.setWill("esp32Mesh/LWT", 2, false, "ESP32 Connection Dropped");
    mqttClient.onMessage(mqttCallback);
    mqttClient.setServer(IP_RASPBERRY, 1883);
    mqttClient.connect();

    while(!mqttClient.connected())
    {
        delay(2000);
        #ifdef DEBUG
            Serial.print('-');
        #endif
    }

    TimerHandle_t timer = xTimerCreate("heartbeatTimer", pdMS_TO_TICKS(5 * 60 * 1000), pdTRUE, (void*) 0, heartbeatTimer);
    xTimerStart(timer, portMAX_DELAY);
}

void MQTT::onMqttConnect(bool sessionPresent)
{
  mqttClient.subscribe("esp32Mesh/to/#", 2);
  mqttClient.publish("esp32Mesh/status", 2, false, "Connected!");
  heartbeatTimer(NULL);
}

void MQTT::mqttCallback(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  char *cleanPayload = (char *)malloc(len + 1);
  memcpy(cleanPayload, payload, len);
  cleanPayload[len] = '\0';
  String msg = String(cleanPayload);
  free(cleanPayload);

  String targetStr = String(topic).substring(13).substring(0, 8);
  if (targetStr == "root")
  {
    if(msg == "getMAC")
      mqttClient.publish("esp32Mesh/MACAddress", 2, false, WiFi.macAddress().c_str());
    else if(msg == "getIP")
      mqttClient.publish("esp32Mesh/IPAddress", 2, false, WiFi.localIP().toString().c_str());
    else if(msg == "RESTART")
      ESP.restart();
  }
  else
  {
    ESPNOW::sendData(msg, targetStr);
  }
}

void MQTT::heartbeatTimer(TimerHandle_t handle)
{
  mqttClient.publish("esp32Mesh/heartbeat", 2, false, "");
}