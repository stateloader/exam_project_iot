#include "server_role.h"

// Server Role

// Makes the data fetched in "client mode" available for other clients. I've written this
// module using Arduino's BLE server example as inspiration, written by Evandro Copercini
// and updated by by chegewara.

// Arduino ESP32 BLE libraries.
static BLEServer *server = NULL;
static BLEService *service = NULL;
static BLEAdvertising *advertising = NULL;
static BLECharacteristic *resource = NULL;

static uint8_t  server_state = 0;
static uint64_t server_stamp = 0;

static void server_setup() {
//Setups the server. Quite a "everything at once" approach at the moment wich I'll deal with
//later.

  Serial.begin(BAUDRATE);
  pinMode(SERVER_PIN, OUTPUT);
  digitalWrite(SERVER_PIN, 1);

  BLEDevice::init(SUPPLY_SERVER_NAME);
  
  server = BLEDevice::createServer();
  Serial.println("server succ - server device created.");

  service = server->createService(TARGET_SERVICE_UUID);
  Serial.println("server succ - service created.");

    resource = service->createCharacteristic(
    TARGET_RESOURCE_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  
  Serial.println("server succ - resource created.");

  resource->addDescriptor(new BLE2902());
  Serial.println("server succ - descriptor added.");
 
  service->start();

  advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(TARGET_SERVICE_UUID);
  
  BLEDevice::startAdvertising();
  Serial.println("server succ - advertising begun.\n");

  server_state = SERVER_STATE_PUSH;
  return;
}

static void server_state_push(void) {
//Updates the charasteristic/resource/data value.

  char push_data[8] = {'\0'};

  sprintf(push_data, "%d", RTC_supply_data);
  Serial.print("server push - updating resource, value: ");
  Serial.print(push_data);
  Serial.print("\n");

  resource->setValue(push_data);

  server_stamp = micros();
  server_state = SERVER_STATE_BUFF;
  return;
}

static void server_state_buff(void) {
//State in place for letting client's catch up with the news.

  if (supply_timer(server_stamp, SERVER_TIMER_BUFF))
    server_state = SERVER_SLEEP_MODE;
  return;
}

static void server_sleep_mode(void) {
//Server going into deep sleep after SERVER_ROLE has been assigned.

  RTC_supply_role = CLIENT_ROLE;
  esp_sleep_enable_timer_wakeup(SLEEP_TIMER_SERVER);
  esp_deep_sleep_start();
  return;
}

static void server_error_mode(void) {
//X_X

  Serial.println("Server Mayday!");
  RTC_supply_role = BROKEN_ROLE;
  esp_sleep_enable_timer_wakeup(uSSEC);
  esp_deep_sleep_start();
}

void server_routine(void) {
//Server SM.

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