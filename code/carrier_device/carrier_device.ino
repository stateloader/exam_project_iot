#include "configs.h"

//Carrier Device.

//This device alternates between client and server roles. Relays data within a local
//BLE network.

void setup() {
  Serial.begin(BAUDRATE);
  carrier_routine();
}

void loop() {}
