#include "configs.h"

bool supply_timer(uint64_t prev_stamp, uint64_t interval) {
//Returns 'true' when the diff between a previous stamp and current 'micros' is equal
//or greater than 'interval'.

  if (!(micros() - prev_stamp >= interval))
    return false;
  return true;
}

static void broken_routine(void) {
  pinMode(BROKEN_PIN, OUTPUT);
  digitalWrite(BROKEN_PIN, 1);
}

void supply_routine(void) {
//A supply routine will be executed based on previous role assigned to 'RCT_supply_role'.

  switch (RTC_supply_role) {

  case CLIENT_ROLE:
  client_routine();
  break;

  case SERVER_ROLE:      
  server_routine(); 
  break;

  case BROKEN_ROLE:
  broken_routine();

  default:
  Serial.println("Jump Table Mayday!");
  broken_routine();
  }
}