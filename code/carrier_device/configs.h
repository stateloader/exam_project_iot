#ifndef CONFIGS__H
#define CONFIGS__H

#include "BLEDevice.h"
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string.h>

#define BAUDRATE 115200

#define SOURCE_SERVER_NAME   "floor1_server"
#define SOURCE_SERVICE_UUID  "8c206542-8d11-11ed-a1eb-0242ac120002"
#define SOURCE_RESOURCE_UUID "94cdef66-8d11-11ed-a1eb-0242ac120002"

#define DEVICE_CLIENT_NAME   "floor3_client"
#define DEVICE_SERVER_NAME   "floor3_server"

#define TARGET_SERVICE_UUID  "40f7c1a4-8f97-11ed-a1eb-0242ac120002"
#define TARGET_RESOURCE_UUID "4cd1a9fe-8f97-11ed-a1eb-0242ac120002"
#define CLIENT_ROLE 0
#define SERVER_ROLE 1
#define BROKEN_ROLE 2

//While the device is in deep sleep mode, everything is powered down and all data is lost. Almost.
//Variable(s) of type 'RTC_DATA_ATTR' (RTC memory) makes it possible to store 8kb of data during
//deep sleep. Utilized for keeping track of roles and relayed data.

RTC_DATA_ATTR uint8_t RTC_carrier_role;
RTC_DATA_ATTR float   RTC_carrier_data;

//Pinout
#define CLIENT_PIN 27
#define SERVER_PIN 26
#define BROKEN_PIN 25

//Timers in uS.
#define uSSEC 1000000
#define uSMIN uSSEC*60

#define SLEEP_TIMER_SERVER uSMIN * 9
#define SLEEP_TIMER_CLIENT uSSEC / 4

#define check_numb(b) (b >= '0' && b <= '9')

bool carrier_timer(uint64_t prev_stamp, uint64_t interval);
void carrier_routine(void);
void broken_routine(void);

#endif
