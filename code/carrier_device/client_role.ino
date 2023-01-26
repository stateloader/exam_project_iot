#include "client_role.h"

// Client Role

// Reads data from a remote server and stores it in RTC memory before going to bed. I've
// written this module using Arduino's BLE client example as inspiration, itself written
// by "Unknown" and updated by "chegewara".

BLEUUID REMOTE_SERVICE_UUID(SOURCE_SERVICE_UUID);
BLEUUID REMOTE_RESOURCE_UUID(SOURCE_RESOURCE_UUID);

// Arduino ESP32 BLE libraries.
static BLEClient *client = NULL;
static BLEAddress *remote_address = NULL;
static BLERemoteService *remote_service = NULL;
static BLERemoteCharacteristic *remote_resource = NULL;

static uint8_t client_state = 0;
static uint8_t client_attmp = 0;
static uint64_t client_stamp = 0;

static bool advertised = false;

class Advertised_Callback: public BLEAdvertisedDeviceCallbacks {
//Callback class invoked if the scanner (in func 'client_state_scan') finds Sensor Device.

  void onResult(BLEAdvertisedDevice advertisedDevice) {

    if (advertisedDevice.getName() != SOURCE_SERVER_NAME) return;
      
    advertisedDevice.getScan()->stop();
    remote_address = new BLEAddress(advertisedDevice.getAddress());
    advertised = true;
  }
};

static void client_state_scan(void) {
//Create Client and scans the surroundings for 'SOURCE_SERVER_NAME', a remote server which
//transmits the data I want to relay.

  BLEDevice::init(DEVICE_CLIENT_NAME);
  BLEScan* scan_server = BLEDevice::getScan();
  scan_server->setAdvertisedDeviceCallbacks(new Advertised_Callback());
  scan_server->setActiveScan(true);
  scan_server->start(10);

  if (advertised) {
    Serial.println("client scan - desired server device found.");
    client_state = CLIENT_STATE_CONN;
    client_attmp = 0;
 
  } else if (client_attmp == 60) {
    Serial.println("client scan - FAILURE; all scan attempts.");
    client_state = CLIENT_ERROR_MODE;
 
  } else {
    Serial.print("client scan - FAILURE; finding server. Attempt: ");
    Serial.print(client_attmp);
    Serial.println(" of 60.");
    client_attmp += 1;
  }
}

static uint8_t client_state_conn_check_service(void) {
//Check if the remote server provides the desired service. If so, assign it.
  
  remote_service = client->getService(REMOTE_SERVICE_UUID);
  if (!remote_service) {
    Serial.println("client fail - assign target service.");
    return 0;
  }
  return 1;
}

static uint8_t client_state_conn_check_reading(void) {
//Check if the remote server provides desired resource. If so, assign it.

  remote_resource = remote_service->getCharacteristic(REMOTE_RESOURCE_UUID);
  if (!remote_resource) {
    Serial.println("client fail - assign sensor data/characteristics.");
    return 0;
  }
  return 1;
}

static void client_state_conn(void) {
//Create and connect to remote device (Sensor Device or another Carrier Device).
  
  client = BLEDevice::createClient();
  client->connect(*remote_address);

  uint8_t check_sercha = 0;
  check_sercha += client_state_conn_check_service();
  check_sercha += client_state_conn_check_reading();

  if (check_sercha == 2 && client->isConnected()) {
    Serial.println("client conn - established.");
    client_state = CLIENT_STATE_READ;
    client_attmp = 0;
    client_stamp = micros();
 
  } else if (client_attmp == 60) {
    Serial.println("client conn - FAILURE; all connection attempts.");
    client_state = CLIENT_ERROR_MODE;  
  
  } else {
    Serial.print("client conn - FAILURE; establishing connection, attempt: ");
    Serial.print(client_attmp);
    Serial.println(" of 60.");
    client_attmp += 1;
  }
}

static void client_state_read(void) {
//Check if we're still connected or a second has passed since last reading.

  float value = remote_resource->readFloat();

  if (!value) return;
  Serial.print("client read - resource fetched, value: ");
  Serial.print(value);
  Serial.print(".\n");

  RTC_carrier_data = value;
  client->disconnect();
  client_state = CLIENT_SLEEP_MODE;  
}

static void client_sleep_mode(void) {
//Client going into deep sleep after SERVER_ROLE variable has been assigned.
  
  RTC_carrier_role = SERVER_ROLE;  
  esp_sleep_enable_timer_wakeup(SLEEP_TIMER_CLIENT);
  esp_deep_sleep_start();
}

static void client_error_mode(void) {
//Error occured during client duty.

  RTC_carrier_role = BROKEN_ROLE;
  broken_routine();
}

void client_routine(void) {
//Statemachine Client Role.

  Serial.println("\nSupply Client Setup\n");
  while (1) {

    switch (client_state) {

    case CLIENT_STATE_SCAN: 
    client_state_scan();
    break;

    case CLIENT_STATE_CONN: 
    client_state_conn();
    break;

    case CLIENT_STATE_READ:
    client_state_read();
    break;

    case CLIENT_SLEEP_MODE:
    client_sleep_mode();

    case CLIENT_ERROR_MODE:
    client_error_mode();

    default:
    client_error_mode();
    }
  }
}
