#include <constants_global.h>
#include <esp_task_wdt.h>
#include <espwifi.h>
#include <espnow.h>
#include <mqtt.h>

TickType_t taskWaitTime;

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  WIFI::setupWiFi();
  MQTT::setupMqtt();
  ESPNOW::setupEspNow();

  esp_task_wdt_init(2, true);
  esp_task_wdt_add(NULL);
  taskWaitTime = xTaskGetTickCount();
}
void loop() {
  esp_task_wdt_reset();
  vTaskDelayUntil(&taskWaitTime, pdMS_TO_TICKS(200));
}