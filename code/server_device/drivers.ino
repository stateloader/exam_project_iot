#include "drivers.h"

//Drivers

//Server Device in circuit with a distance sensor (HC-SR05) responsible for reading waste
//levels in the context of this "Effektivt soprum"-project.

BLEServer *server = NULL;
BLEService *service = NULL;
BLEAdvertising *advertising = NULL;
BLECharacteristic *resource = NULL;

static uint8_t  server_state = 0;
static uint8_t  sample_count = 0;
static uint64_t server_stamp = 0;

static uint64_t duration = 0;
static uint16_t distance = 0;

static bool server_timer(uint64_t prev_stamp, uint64_t interval) {
//Returns 'true' when the diff between a previous stamp and current 'micros' is equal
//or greater than 'interval'.

  if (!(micros() - prev_stamp >= interval))
    return false;
  return true;
}

static void server_configs(void) {
//Setups the server. Quite a "everything at once" approach at the moment wich I'll deal
//with later.

  Serial.begin(BAUDRATE);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  BLEDevice::init(SENSOR_SERVER_NAME);
  server = BLEDevice::createServer();
  Serial.println("server succ - device created.");

  service = server->createService(SERVER_SERVICE_UUID);
  Serial.println("server succ - service created.");

  resource = service->createCharacteristic(
    SERVER_RESOURCE_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );

  Serial.println("server succ - resource created.");

  resource->addDescriptor(new BLE2902());
  Serial.println("server succ - descriptor added.");
 
  service->start();

  advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVER_SERVICE_UUID);
  
  BLEDevice::startAdvertising();
  Serial.println("server succ - advertising begun\n");

  server_stamp = micros();
  return;
}

static void server_state_init(void) {
//Just digital placebo I'm going to ditch later.

  if (server_timer(server_stamp, SERVER_INIT_TIMER))
    server_state = SERVER_STATE_READ;
  return;
}

static void server_state_read(void) {
//'TRIG_PIN' emits a high-frequency sound wave which "bounces" back from an object.
//'ECHO_PIN' receives this "bounce". The duration of this process, emit to receive,
// is stored in 'duration' and used for calculating distances (in cm).

  digitalWrite(TRIG_PIN, LOW);

  server_stamp = micros();
  while(!server_timer(server_stamp, 2));

  digitalWrite(TRIG_PIN, HIGH);

  server_stamp = micros();
  while(!server_timer(server_stamp, 10));

  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (uint16_t) duration * SOUND_SPEED / 2;

  server_stamp = micros();
  server_state = SERVER_STATE_PUSH;
  return;
}

static void server_state_push(void) {
//Write the third sensor value to the characteristic/resource. The very first one is
//always 0 (due to current conf) and the second is just for <input reason>.

  ++sample_count;
  if (sample_count < 3) {
    server_state = SERVER_STATE_INIT;
    return;
  }

  char push_data[8] = {'\0'};

  sprintf (push_data, "%d", distance);
  Serial.println("server data - updating resource, value: ");
  Serial.print(push_data);
  Serial.print(".\n");

  resource->setValue(push_data);

  server_state = SERVER_STATE_BUFF;
  return;
}

static void server_state_buff(void) {
//A time buffer for letting listening nodes be aware there's some action going on.
  
  if (server_timer(server_stamp, SERVER_BUFF_TIMER))
    server_state = SERVER_SLEEP_MODE;
  return;
}

static void server_sleep_mode(void) {
//Server going into deep sleep 'SERVER_SLEEP_TIMER' amount of time.

  esp_sleep_enable_timer_wakeup(SERVER_SLEEP_TIMER);
  esp_deep_sleep_start();
}

void server_routine(void) {
//SM Server.

  server_configs();

  while (1) {
    
    switch (server_state) {

    case SERVER_STATE_INIT:
    server_state_init();
    break;

    case SERVER_STATE_READ:
    server_state_read();
    break;

    case SERVER_STATE_PUSH:
    server_state_push();
    break;

    case SERVER_STATE_BUFF:
    server_state_buff();
    break;

    case SERVER_SLEEP_MODE:
    server_sleep_mode();

    default:
    return;
    }
  }
}