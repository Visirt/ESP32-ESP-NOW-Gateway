#pragma once
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <constants_mqtt.h>

#define MQTT_NAME "esp32Client"

extern AsyncMqttClient mqttClient;

void setupMqtt();
void onMqttConnect(bool sessionPresent);
void heartbeatTimer(TimerHandle_t handle);
