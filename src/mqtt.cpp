#include <mqtt.h>

AsyncMqttClient mqttClient;

void setupMqtt()
{
    mqttClient.setClientId(MQTT_NAME);
    mqttClient.setCredentials(MQTT_USER, MQTT_PASSWD);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.setKeepAlive(30);
    mqttClient.setWill("esp32Mesh/LWT", 2, false, "ESP32 Connection Dropped");
    // mqttClient.onMessage(mqttCallback);
    mqttClient.setServer(IP_RASPBERRY, 1883);
    mqttClient.connect();

    while(!mqttClient.connected())
    {
        delay(2000);
        #ifdef DEBUG
            Serial.print('-');
        #endif
    }

    xTimerCreate("heartbeatTimer", pdMS_TO_TICKS(5 * 60 * 1000), pdTRUE, (void*) 0, heartbeatTimer);
}

void onMqttConnect(bool sessionPresent)
{
  // mqttClient.subscribe("esp32Mesh/to/#", 2);
  mqttClient.publish("esp32Mesh/status", 2, false, "Connected!");
  heartbeatTimer(NULL);
}

void heartbeatTimer(TimerHandle_t handle)
{
  mqttClient.publish("esp32Mesh/heartbeat", 2, false, "");
}