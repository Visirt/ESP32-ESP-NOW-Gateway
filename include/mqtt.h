#pragma once
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <constants_mqtt.h>
#include <WiFi.h>

#define MQTT_NAME "esp32Client"

extern AsyncMqttClient mqttClient;

void setupMqtt();
void onMqttConnect(bool sessionPresent);
void mqttCallback(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void heartbeatTimer(TimerHandle_t handle);
