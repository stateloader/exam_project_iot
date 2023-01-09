#include "configs.h"

// Supply Device

// This device alternates between client and server roles. Why I've come up with this weird
// solution will be explained in the final documentation but it has to do with relaying data
// from a remote sensor out of radio range.

void setup() {
  supply_routine();
}

void loop() {}