#include "configs.h"

bool carrier_timer(uint64_t prev_stamp, uint64_t interval) {
//Return 'true' when the diff between a previous stamp and current 'micros' is equal
//or greater than 'interval'.

  if (!(micros() - prev_stamp >= interval))
    return false;
  return true;
}

void broken_routine(void) {
//The device "error led" (red) going to shine along with the role in wich something
//went south.

  uint8_t broken_state = 0;
  pinMode(BROKEN_PIN, OUTPUT);
  digitalWrite(BROKEN_PIN, 1);

  uint64_t stamp = micros();

  while (!carrier_timer(stamp, uSSEC * 5));
  
  esp_sleep_enable_timer_wakeup(uSSEC);
  esp_deep_sleep_start();
}

void carrier_routine(void) {
//A supply routine will be executed based on previous role assigned to 'RTC_supply_role'.

  switch (RTC_carrier_role) {

  case CLIENT_ROLE:
  pinMode(CLIENT_PIN, OUTPUT);
  digitalWrite(CLIENT_PIN, 1);
  client_routine();
  break;

  case SERVER_ROLE:
  pinMode(SERVER_PIN, OUTPUT);
  digitalWrite(SERVER_PIN, 1);
  server_routine(); 
  break;

  default:
  broken_routine();

  }
}
