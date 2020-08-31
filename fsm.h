#pragma once

#include <arduino.h> // uint8_t types

#define FSM_DEBUG

/* ESP8266 has quite a few pins but most of them are reserved or used for
 * internal stuff like programming-mode and communication with the flash
 * module.
 * So we have to use as less pins as possible. As we need two for motor-control
 * we also use one of them for the status LED.
 * Pin1 / Pin2 -> Function
 *  L      L      Lid closed,  LED off
 *  H      L      Lid opening, LED on
 *  H      H      Lid open,    LED on
 *  L      H      Lid closing, LED off
 */
#define MOTOR1 D4 // also used for LED
#define MOTOR2 D3

#define MOTOR_OPEN_TIME 6000 // in ms
#define MOTOR_CLOSE_TIME 6000 // in ms

// Make sure to always update names in fsm.cpp too!
enum State {
    FSM_INIT = 0,
    FSM_CLOSED,
    FSM_AUTO_OPENING,
    FSM_AUTO_OPEN,
    FSM_MANUAL_OPENING,
    FSM_MANUAL_OPEN,
    FSM_CLOSING,
    FSM_ERROR
};

extern const char* state_strings[];

extern enum State current_state;

// Public methods
void init_fsm();
void handle_fsm(bool should_be_state, bool override_state);

// Helper methods
void motor_open();
void motor_stop();
void motor_close();

// Handle methods for all states
enum State handle_init(bool entered, bool should_be_state, bool override_state);
enum State handle_closed(bool entered, bool should_be_state, bool override_state);
enum State handle_auto_opening(bool entered, bool should_be_state, bool override_state);
enum State handle_auto_open(bool entered, bool should_be_state, bool override_state);
enum State handle_manual_opening(bool entered, bool should_be_state, bool override_state);
enum State handle_manual_open(bool entered, bool should_be_state, bool override_state);
enum State handle_closing(bool entered, bool should_be_state, bool override_state);
enum State handle_error(bool entered, bool should_be_state, bool override_state);
