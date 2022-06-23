#pragma once
#include <Arduino.h>
#include <esp_now.h>
#include <mqtt.h>

void setupEspNow();
void onRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len);
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status);
String macAddrString(const uint8_t * mac_addr);
esp_err_t addSlave(uint8_t* address);
esp_err_t sendData(String message, String topic, uint8_t* dest);