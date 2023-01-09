#ifndef CONFIGS__H
#define CONFIGS__H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string.h>
#include <stdint.h>

#define BAUDRATE 115200

//https://www.uuidgenerator.net/version1

#define SENSOR_SERVER_NAME   "SERVER_DEVICE_V1"
#define SERVER_SERVICE_UUID  "3b54ddce-8aa4-11ed-a1eb-0242ac120002"
#define SERVER_RESOURCE_UUID "a196c782-8aa4-11ed-a1eb-0242ac120002"

#define uSSEC 1000000
#define uSMIN uSSEC*60

//Test Intervals for now.
#define SERVER_INIT_TIMER  uSSEC
#define SERVER_BUFF_TIMER  uSSEC
#define SERVER_SLEEP_TIMER uSSEC

#define BAUDRATE 115200
#define TRIG_PIN 5
#define ECHO_PIN 18

#define SOUND_SPEED 0.034

#endif