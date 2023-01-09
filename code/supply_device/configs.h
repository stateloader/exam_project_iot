#ifndef CONFIGS__H
#define CONFIGS__H

#include "BLEDevice.h"
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string.h>

#define BAUDRATE 115200

//https://www.uuidgenerator.net/version1

//'SOURCE' is name, service and resource info about the the device/node where the Supply Device
//gathers it data from.
#define SOURCE_SERVER_NAME   "FLOOR1_SERVER"
#define SOURCE_SERVICE_UUID  "8c206542-8d11-11ed-a1eb-0242ac120002"
#define SOURCE_RESOURCE_UUID "94cdef66-8d11-11ed-a1eb-0242ac120002"

//Names used (and advertised within its radio range) as the device alternates between client and
//server roles.
#define SUPPLY_CLIENT_NAME   "FLOOR3_CLIENT"
#define SUPPLY_SERVER_NAME   "FLOOR3_SERVER"

//'TARGET' is service and resource info about where connected nodes go for gathering data
//supplied by the (THIS) device (relayed from "SOURCE").
#define TARGET_SERVICE_UUID  "40f7c1a4-8f97-11ed-a1eb-0242ac120002"
#define TARGET_RESOURCE_UUID "4cd1a9fe-8f97-11ed-a1eb-0242ac120002"

#define CLIENT_ROLE 0
#define SERVER_ROLE 1
#define BROKEN_ROLE 2

//RCT_DATA_ATTR.

//While the device is in deep sleep mode, everything is powered down and all data is lost. Almost.
//Variable(s) of type 'RTC_DATA_ATTR' (RCT memory) make it possible to store 8kb of data during
//deep sleep. Utilized for keeping track of roles and relayed data.

RTC_DATA_ATTR uint16_t RTC_supply_role;
RTC_DATA_ATTR uint16_t RTC_supply_data;

//Pinout
#define CLIENT_PIN 16
#define SERVER_PIN 18
#define BROKEN_PIN 21

//Timers in uS.
#define uSSEC 1000000
#define uSMIN uSEC*60

//Test Intervals for now.
#define SLEEP_TIMER_SERVER uSSEC * 5
#define SLEEP_TIMER_CLIENT uSSEC * 4

#define ATTEMPTS 60
#define check_numb(b) (b >= '0' && b <= '9')

bool supply_timer(uint64_t prev_stamp, uint64_t interval);
void supply_routine(void);

#endif