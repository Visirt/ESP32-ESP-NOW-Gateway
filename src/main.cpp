#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <esp_task_wdt.h>
#include <constants_wifi.h>
#include <constants_mqtt.h>

// Device Name
#define WIFI_NAME "ESP32_NET"
#define MQTT_NAME "esp32Client"

void onMqttConnect(bool sessionPresent);
void heartbeatTimer(TimerHandle_t handle);

IPAddress mqttBroker(IP_RASPBERRY);

AsyncMqttClient mqttClient;

String macAddrString(const uint8_t * mac_addr)
{
  return String(mac_addr[0], HEX) + ":" + String(mac_addr[1], HEX) + ":" + String(mac_addr[2], HEX) + ":" + String(mac_addr[3], HEX) + ":" + String(mac_addr[4], HEX) + ":" + String(mac_addr[5], HEX);
}

void OnRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  String data = String((char*)incomingData);
  int semiColonLocation = data.indexOf(';');
  int numItems = data.substring(0, semiColonLocation).toInt();
  String topic = data.substring(semiColonLocation + 1, numItems + semiColonLocation + 1);
  if(topic.isEmpty())
    topic = "from";
  String message = data.substring(semiColonLocation + numItems + 1);
  mqttClient.publish(("esp32Mesh/" + topic + "/" + macAddrString(mac_addr)).c_str(), 2, false, message.c_str());
}

void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(status != ESP_NOW_SEND_SUCCESS)
    Serial.println("ERROR SENDING DATA");
}
esp_err_t addSlave(uint8_t* address){
  esp_now_peer_info_t slaveInfo;
  memcpy(slaveInfo.peer_addr, address, 6);
  slaveInfo.channel = 0;  
  slaveInfo.encrypt = false;
  
  return esp_now_add_peer(&slaveInfo);
}

esp_err_t SendData(String message, String topic, uint8_t* dest){
  String sendingData = String(topic.length()) + ";" + topic + message;
  Serial.println("Sending Data:" + sendingData);
  return esp_now_send(dest, (uint8_t*)sendingData.c_str(), sendingData.length());
}

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(WIFI_NAME);
  esp_wifi_set_ps(WIFI_PS_NONE);
  
  WiFi.begin(SSID_Router, PASSWD_Router);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  mqttClient.setClientId(MQTT_NAME);
  mqttClient.setCredentials(MQTT_USER, MQTT_PASSWD);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.setKeepAlive(30);
  mqttClient.setWill("esp32Mesh/LWT", 2, false, "ESP32 Connection Dropped");
  // mqttClient.onMessage(mqttCallback);
  mqttClient.setServer(mqttBroker, 1883);

  while(!mqttClient.connected())
  {
    delay(2000);
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("There was an error initializing ESP-NOW");
    delay(10000);
    ESP.restart();
    return;
  }
  esp_now_register_send_cb(OnSent);
  esp_now_register_recv_cb(OnRecv);

  xTimerCreate("heartbeatTimer", pdMS_TO_TICKS(5 * 60 * 1000), pdTRUE, (void*) 0, heartbeatTimer);


  esp_task_wdt_init(5, true);
  esp_task_wdt_add(NULL);
}
void loop() {
  esp_task_wdt_feed();
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