#include "drivers.h"

//Drivers

BLEServer *server = NULL;
BLEService *service = NULL;
BLEAdvertising *advertising = NULL;
BLECharacteristic *resource = NULL;

static uint8_t  sensor_state = 0;
static uint64_t sensor_stamp = 0;

static uint64_t duration = 0;
static float    distance = 0;
static uint8_t  attempts = 0;

static bool sensor_timer(uint64_t prev_stamp, uint64_t interval) {
//Return 'true' when the diff between a previous stamp and current 'micros' is equal
//or greater than 'interval'.

  if (!(micros() - prev_stamp >= interval))
    return false;
  return true;
}

static void sensor_configs(void) {
//Setup the sensor server. Quite a "everything at once" approach at the moment wich
//I hopefully will deal with later.

  BLEDevice::init(SENSOR_SERVER_NAME);
  server = BLEDevice::createServer();
  Serial.println("server conf - device created.");

  service = server->createService(SENSOR_SERVICE_UUID);
  Serial.println("server conf - service created.");

  resource = service->createCharacteristic(
    SENSOR_RESOURCE_UUID, BLECharacteristic::PROPERTY_READ
  );

  Serial.println("server conf - resource created.");

  resource->addDescriptor(new BLE2902());
  Serial.println("server conf - descriptor added.");
 
  service->start();

  advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SENSOR_SERVICE_UUID);
  
  BLEDevice::startAdvertising();
  Serial.println("server conf - advertising begun\n");

  sensor_stamp = micros();
  return;
}

static void sensor_state_init(void) {
//Just digital placebo I'm going to ditch later.

  if (sensor_timer(sensor_stamp, SENSOR_INIT_TIMER))
    sensor_state = SENSOR_STATE_READ;
  return;
}

static void sensor_state_read(void) {
//'TRIG_PIN' emits a high-frequency sound wave which "bounces" back from an object.
//'ECHO_PIN' receives this "bounce". The duration of this process, emit to receive,
// is stored in 'duration' and used for calculating distances (in cm).

  digitalWrite(TRIG_PIN, LOW);

  sensor_stamp = micros();
  while (!sensor_timer(sensor_stamp, 2));

  digitalWrite(TRIG_PIN, HIGH);

  sensor_stamp = micros();
  while (!sensor_timer(sensor_stamp, 10));

  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * SOUND_SPEED / 2;
  
  sensor_stamp = micros();
  sensor_state = SENSOR_STATE_PUSH;
  return;
}

static void sensor_state_push(void) {
//If sensor reading is < 1, it's probably the first reading ("the flush"), else it's
//most of the cases a decent value.

  if (distance == NULL) {
    Serial.println("server buff - flush reading.");
    sensor_state = SENSOR_STATE_INIT;
    sensor_stamp = micros();    

  } else if (distance < 1) {
    Serial.println("server fail - value probably minus for some reason.");
    sensor_state = SENSOR_STATE_INIT;
    sensor_stamp = micros();

  } else {
    Serial.print("server data - updating resource, waste level: ");
    Serial.print(distance);
    resource->setValue(distance);
    sensor_state = SENSOR_STATE_BUFF;
  }
}

static void sensor_state_buff(void) {
//Time buffer for letting listening nodes be aware there's some action going on.
  
  if (sensor_timer(sensor_stamp, SENSOR_BUFF_TIMER))
    sensor_state = SENSOR_SLEEP_MODE;
  return;
}

static void sensor_sleep_mode(void) {
//Server going into deep sleep 'SENSOR_SLEEP_TIMER' amount of time.

  Serial.println("\nsensor succ - sensor reading done, sleep mode for 10 minutes.");
  esp_sleep_enable_timer_wakeup(SENSOR_SLEEP_TIMER);
  esp_deep_sleep_start();
}

void sensor_routine(void) {
//Sensor Device statemachine.

  Serial.begin(BAUDRATE);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  Serial.println("\nEndpoint Server Setup\n");
  
  sensor_configs();

  while (1) {
    
    switch (sensor_state) {

    case SENSOR_STATE_INIT:
    sensor_state_init();
    break;

    case SENSOR_STATE_READ:
    sensor_state_read();
    break;

    case SENSOR_STATE_PUSH:
    sensor_state_push();
    break;

    case SENSOR_STATE_BUFF:
    sensor_state_buff();
    break;

    case SENSOR_SLEEP_MODE:
    sensor_sleep_mode();
  
    default:
    return;
    }
  }
}
