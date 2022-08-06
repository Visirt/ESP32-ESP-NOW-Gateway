#include <constants_global.h>
#include <esp_task_wdt.h>
#include <espwifi.h>
#include <espnow.h>
#include <mqtt.h>
#include <EspNowFloodingMesh.h>
#include <ArduinoOTA.h>

TickType_t taskWaitTime;

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  ESPNOW::setupEspNow();
  WIFI::setupWiFi();
  MQTT::setupMqtt();

  ArduinoOTA.onStart([]() {
    esp_task_wdt_delete(NULL);
  });
  ArduinoOTA.begin();

  esp_task_wdt_init(2, true);
  esp_task_wdt_add(NULL);
  taskWaitTime = xTaskGetTickCount();
}
void loop() {
  esp_task_wdt_reset();
  espNowFloodingMesh_loop();
  ArduinoOTA.handle();
}