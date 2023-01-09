#include "client_role.h"

// Client Role

// Reads data from a remote server/node and stores it in RCT memory before going to bed. I've
// written this module using Arduino's BLE client example as inspiration, written by "Unknown"
// and updated by "chegewara".

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

static void client_setup() {

  Serial.begin(BAUDRATE);
  pinMode(CLIENT_PIN, OUTPUT);
  digitalWrite(CLIENT_PIN, 1);
}

class Advertised_Callback: public BLEAdvertisedDeviceCallbacks {
//Callback class invoked if the scanner (in func 'client_state_scan') finds the Sensor Device.

  void onResult(BLEAdvertisedDevice advertisedDevice) {

    if (advertisedDevice.getName() != SOURCE_SERVER_NAME) return;
      
    advertisedDevice.getScan()->stop();
    remote_address = new BLEAddress(advertisedDevice.getAddress());
    advertised = true;
  }
};

static void client_state_scan(void) {
//Client being created and scans the surroundings for 'SOURCE_SERVER_NAME', a remote server
//which transmits the data I want to relay.

  BLEDevice::init(SUPPLY_CLIENT_NAME);
  BLEScan* scan_server = BLEDevice::getScan();
  scan_server->setAdvertisedDeviceCallbacks(new Advertised_Callback());
  scan_server->setActiveScan(true);
  scan_server->start(10);

  if (advertised) {
    Serial.println("client succ - desired server device found.");
    client_state = CLIENT_STATE_CONN;
    client_attmp = 0;
 
  } else if (client_attmp == 60) {
    Serial.println("client error - failed to find server.");
    client_state = CLIENT_ERROR_MODE;
 
  } else {
    Serial.print("client fail - find server, attempt ");
    Serial.print(client_attmp);
    Serial.println(" of 60.");
    client_attmp += 1;
  }
}

static uint8_t client_state_conn_check_service(void) {
//Checks if the remote server provides the desired service. If so, assign it.
  
  remote_service = client->getService(REMOTE_SERVICE_UUID);
  if (!remote_service) {
    Serial.println("client fail - assign target service.");
    return 0;
  }
  return 1;
}

static uint8_t client_state_conn_check_reading(void) {
//Checks if the remote server provides desired resource. If so, assign it.

  remote_resource = remote_service->getCharacteristic(REMOTE_RESOURCE_UUID);
  if (!remote_resource) {
    Serial.println("client fail - assign sensor data/characteristics.");
    return 0;
  }
  return 1;
}

static void client_state_conn(void) {
//Creates and connects/pairing the device to the remote server.
  
  client = BLEDevice::createClient();
  client->connect(*remote_address);

  uint8_t check_sercha = 0;
  check_sercha += client_state_conn_check_service();
  check_sercha += client_state_conn_check_reading();

  if (check_sercha == 2 && client->isConnected()) {
    Serial.println("client succ - connection established.");
    client_state = CLIENT_STATE_READ;
    client_attmp = 0;
    client_stamp = micros();
 
  } else if (client_attmp == 60) {
    Serial.println("client error - failed to establish connection.");
    client_state = CLIENT_ERROR_MODE;  
  
  } else {
    Serial.print("client fail - establishing connection, attempt ");
    Serial.print(client_attmp);
    Serial.println(" of 60.");
    client_attmp += 1;
  }
}

static void client_state_read_check_string(void) {
//Reads the characteristic value, i.e: data provided from the remote server. If its empty,
//larger than 8 chars or consist of anything else than numbers (and a terminator) it won't
//be stored.

  std::string value = remote_resource->readValue();

  if (value.empty()) {
    return;
  } else if (value.length() > 7) {
    return;
  } for (uint8_t i = 0; value.c_str()[i] == '\0'; i++) {
    if (!check_numb(value.c_str()[i])) return;
  }
  uint16_t check_value = (uint16_t) atoi(value.c_str());
  if (check_value < 1) return;
  
  RTC_supply_data = check_value;
  Serial.print("client succ - data received: ");
  Serial.print(RTC_supply_data);
  Serial.print(".\n");
}

static void client_state_read(void) {
//Checks if we're still connected or a second has passed since last reading before a new
//sample being picked.

  if (!client->isConnected()) {
    client_state = CLIENT_SLEEP_MODE;
    return;
  } else if (!supply_timer(client_stamp, uSSEC)) {
    return;
  } else {
    client_state_read_check_string();
    client_stamp = micros();
  }
}

static void client_sleep_mode(void) {
//Client going into deep sleep after SERVER_ROLE has been assigned.
  
  RTC_supply_role = SERVER_ROLE;  
  esp_sleep_enable_timer_wakeup(SLEEP_TIMER_CLIENT);
  esp_deep_sleep_start();
}

static void client_error_mode(void) {
//X_X

  Serial.println("Client Mayday!");
  RTC_supply_role = BROKEN_ROLE;  
  esp_sleep_enable_timer_wakeup(uSSEC);
  esp_deep_sleep_start();
}

void client_routine(void) {
//Client SM.

  client_setup();

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


