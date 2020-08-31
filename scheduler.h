#pragma once

#include <arduino.h> // uint8_t types

// Datatype for one open/close time
struct open_close_time_t {
    uint8_t open_hour;
    uint8_t open_minute;
    uint16_t duration; // minutes
};

//#define SCHEDULER_DEBUG

// Configuration of open/close times
#define OPEN_CLOSE_TIMES \
    /* h, min, duration */ \
    {  8,  0, 15 },   \
    { 10,  0, 15 },   \
    { 10, 35,  1 },   \
    { 12, 30, 15 },   \
    { 14, 35,  5 },   \
    { 16, 55,  1 },   \
    { 20, 15,  2 },   \
    { 21, 40,  2 },   \
    { 23, 59,  2 }

// Public methods
void init_scheduler();
bool handle_scheduler();

// Helper
void output_schedules();
uint16_t current_minute();