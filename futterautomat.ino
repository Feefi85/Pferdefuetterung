#include "time.h"
#include "scheduler.h"
#include "ota.h"
#include "fsm.h"

/*
 * D0 (Output Only)
 * D1 SCL
 * D2 SDA
 * D3 Motor2
 * D4 Motor1 (internal LED, inverted as it is connected from D4 to 3V3)
 */

#define SWITCH D0

void setup() {
  Serial.begin(115200);
  Serial.println("\r\n\r\nFutterbox is starting up...");

  init_ota();
  init_time();
  init_scheduler();

  pinMode(SWITCH, INPUT_PULLDOWN_16);

  init_fsm();
}

bool override_switch_debounce() {
  static bool switch_state = false;       // Current switch state after debouncing
  static unsigned long last_switched = 0; // Last switch change
  bool last_switch = digitalRead(SWITCH); // Most current switch reading
  if (last_switch != switch_state &&
      millis() - last_switched > 100) {
      last_switched = millis();
      switch_state = last_switch;
      if(switch_state) {
        Serial.println("-- Button pressed");
      } else {
        Serial.println("-- Button released");
      }
    }
  return switch_state;
}

void loop() {
  handle_ota();
  handle_time();
  bool should_be_state = handle_scheduler();

  bool override_state = override_switch_debounce();
  handle_fsm(should_be_state, override_state);
}