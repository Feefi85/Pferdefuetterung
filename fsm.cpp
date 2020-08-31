/*
 * Methods for handling finite state machine and motor for lid.
 */
#include "fsm.h"

// Current and last state
enum State current_state = FSM_INIT;
enum State last_state = FSM_ERROR; // Set to error to make sure we always have a changed-state-event when entering FSM_INIT

// String-list of states for outputting to serial console
// Make sure to always update enum in state.h too!
const char* state_strings[] = {
    "FSM_INIT",
    "FSM_CLOSED",
    "FSM_AUTO_OPENING",
    "FSM_AUTO_OPEN",
    "FSM_MANUAL_OPENING",
    "FSM_MANUAL_OPEN",
    "FSM_CLOSING",
    "FSM_ERROR"
};


/**
 * Helper for controlling motor.
 */
bool is_lid_closed = false;
void motor_open() {
    Serial.println("Motor: Opening lid...");
    digitalWrite(MOTOR1, LOW);
    digitalWrite(MOTOR2, HIGH);
    is_lid_closed = true;
}

void motor_stop() {
    Serial.println("Motor: Stopping motor");
    digitalWrite(MOTOR1, is_lid_closed);
    digitalWrite(MOTOR2, is_lid_closed);
}

void motor_close() {
    Serial.println("Motor: Closing lid...");
    digitalWrite(MOTOR1, HIGH);
    digitalWrite(MOTOR2, LOW);
    is_lid_closed = false;
}

// since when we are in current state
unsigned long state_entered = 0;


/**
 * init_fsm()
 * Prepares motor output pins
 */
void init_fsm() {
    Serial.println("== init_fsm(): called");
    pinMode(MOTOR1, OUTPUT);
    pinMode(MOTOR2, OUTPUT);
}


/**
 * handle_fsm(…)
 * Checks if state changed, call the currently active handler and check
 * result.
 */
void handle_fsm(bool should_be_state, bool override_state) {

    // Handle detection of state changes
    bool changed = false;
    if (current_state != last_state) {
        changed = true;
        last_state = current_state;
        state_entered = millis();
        Serial.printf("FSM: Entering state: %s\n", state_strings[current_state]);
    }
    
    // give control to method for current state
    enum State next_state;
    switch(current_state) {
        case FSM_INIT:           next_state = handle_init(changed, should_be_state, override_state); break;
        case FSM_CLOSED:         next_state = handle_closed(changed, should_be_state, override_state); break;
        case FSM_AUTO_OPENING:   next_state = handle_auto_opening(changed, should_be_state, override_state); break;
        case FSM_AUTO_OPEN:      next_state = handle_auto_open(changed, should_be_state, override_state); break;
        case FSM_MANUAL_OPENING: next_state = handle_manual_opening(changed, should_be_state, override_state); break;
        case FSM_MANUAL_OPEN:    next_state = handle_manual_open(changed, should_be_state, override_state); break;
        case FSM_CLOSING:        next_state = handle_closing(changed, should_be_state, override_state); break;
        default: /* FSM_ERROR */ next_state = handle_error(changed, should_be_state, override_state); break;
    }

    if (next_state != current_state) {
        Serial.printf("FSM: Requested change from %s to %s\n", state_strings[current_state], state_strings[next_state]);
        current_state = next_state;
    }
}


/**
 * handle_…()
 * All state handlers. All of them are built the same.
 * 1. Check if `entered` and make sure to init all for current state.
 * 2. Do whatever is needed in this state.
 * 3. Check if we need to move to next state and set return value
 */
enum State handle_init(bool entered, bool should_be_state, bool override_state) {
    // Wait a bit to allow for NTP to sync time (if possible)
    // Switch to closing afterwards to go to a known state
    if (millis() - state_entered > 2000) {
        return FSM_CLOSING;
    } else {
        return current_state;
    }
}

enum State handle_closed(bool entered, bool should_be_state, bool override_state) {
    if (entered) {
        motor_stop();
    }
    if (should_be_state) {
        Serial.println("Should be open according to schedule");
        return FSM_AUTO_OPENING;
    } else if (override_state) {
        Serial.println("Button pressed for override");
        return FSM_MANUAL_OPENING;
    } else {
        return current_state;
    }
}

enum State handle_auto_opening(bool entered, bool should_be_state, bool override_state) {
    if (entered) {
        motor_open();
    }
    if (millis() - state_entered > MOTOR_OPEN_TIME) {
        return FSM_AUTO_OPEN;
    } else {
        return current_state;
    }
}

enum State handle_auto_open(bool entered, bool should_be_state, bool override_state) {
    if (entered) {
        motor_stop();
    }
    if (! should_be_state) {
        Serial.println("Should be closed according to schedule");

        // Make sure we don't close if manual override is pressed!
        if (override_state) {
            return FSM_MANUAL_OPEN;
        } else {
            return FSM_CLOSING;
        }
    } else {
        return current_state;
    }
}

enum State handle_manual_opening(bool entered, bool should_be_state, bool override_state) {
    if (entered) {
        motor_open();
    }
    if (millis() - state_entered > MOTOR_OPEN_TIME) {
        return FSM_MANUAL_OPEN;
    } else {
        return current_state;
    }
}

enum State handle_manual_open(bool entered, bool should_be_state, bool override_state) {
    if (entered) {
        motor_stop();
    }
    if (! override_state) {
        Serial.println("Override-button released");
        return FSM_CLOSING;
    } else {
        return current_state;
    }
}

enum State handle_closing(bool entered, bool should_be_state, bool override_state) {
    if (entered) {
        motor_close();
    }
    if (millis() - state_entered > MOTOR_CLOSE_TIME) {
        return FSM_CLOSED;
    } else {
        return current_state;
    }
}

// error-state not implemented
enum State handle_error(bool entered, bool should_be_state, bool override_state) {
    // TODO: Blink status LED
    return current_state;
}