#include "server_role.h"

// Server Role

// Makes the data fetched in "client mode" available for other clients. I've written this
// module using Arduino's BLE server example as inspiration, which itself was written by
// Evandro Copercini and updated by "chegewara".

// Arduino ESP32 BLE libraries.
static BLEServer *server = NULL;
static BLEService *service = NULL;
static BLEAdvertising *advertising = NULL;
static BLECharacteristic *resource = NULL;

static uint8_t  server_state = 0;
static uint64_t server_stamp = 0;

static void server_setup() {
//Setup the server.

  BLEDevice::init(DEVICE_SERVER_NAME);
  
  server = BLEDevice::createServer();
  Serial.println("server init - server device created.");

  service = server->createService(TARGET_SERVICE_UUID);
  Serial.println("server init - service created.");

    resource = service->createCharacteristic(
    TARGET_RESOURCE_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  
  Serial.println("server init - resource created.");

  resource->addDescriptor(new BLE2902());
  Serial.println("server init - descriptor added.");
 
  service->start();

  advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(TARGET_SERVICE_UUID);
  
  BLEDevice::startAdvertising();
  Serial.println("server init - advertising begun.\n");

  server_state = SERVER_STATE_PUSH;
  return;
}

static void server_state_push(void) {
//Updates the charasteristic/resource/data value.

  Serial.print("server push - updating resource, value: ");
  Serial.print(RTC_carrier_data);
  Serial.print("\n");

  resource->setValue(RTC_carrier_data);

  server_stamp = micros();
  server_state = SERVER_STATE_BUFF;
  return;
}

static void server_state_buff(void) {
//Digital placebo for now.

  if (carrier_timer(server_stamp, SERVER_TIMER_BUFF))
    server_state = SERVER_SLEEP_MODE;
  return;
}

static void server_sleep_mode(void) {
//Server going into deep sleep after CLIENT_ROLE has been assigned into RCT memory.

  RTC_carrier_role = CLIENT_ROLE;
  esp_sleep_enable_timer_wakeup(SLEEP_TIMER_SERVER);
  esp_deep_sleep_start();
  return;
}

static void server_error_mode(void) {
//Error occured during server duty.

  RTC_carrier_role = BROKEN_ROLE;
  broken_routine();
}

void server_routine(void) {
//Statemachine Server.
  Serial.println("\nSupply Server Setup\n");

  server_setup();

  while (1) {

    switch (server_state) {

    case SERVER_STATE_PUSH:
    server_state_push();
    break;

    case SERVER_STATE_BUFF:
    server_state_buff();
    break;

    case SERVER_SLEEP_MODE:
    server_sleep_mode();

    case SERVER_ERROR_MODE:
    server_error_mode();

    default:
    server_error_mode();    
    }
  }
}
