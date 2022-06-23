#include <esp_task_wdt.h>
#include <espwifi.h>
#include <espnow.h>
#include <mqtt.h>


// #define DEBUG

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  setupWiFi();
  setupMqtt();
  setupEspNow();

  esp_task_wdt_init(5, true);
  esp_task_wdt_add(NULL);
}
void loop() {
  esp_task_wdt_reset();
}