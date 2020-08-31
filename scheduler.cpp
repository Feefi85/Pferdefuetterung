/*
 * Methods for handling schedule of open/close times. Does not do the opening/closing!
 */
#include "scheduler.h"

// Configured list of open-times and duration, see scheduler.h
struct open_close_time_t open_close[] = { OPEN_CLOSE_TIMES };


/**
 * init_scheduler()
 * Reads configured schedules and outputs them to serial.
 */
void init_scheduler() {
    Serial.println("== init_scheduler(): called");
    output_schedules();
}


/**
 * handle_scheduler()
 * Needs to be called often.
 * Calculates if lid should be open or closed. Works by calculating the number of minutes
 * since midnight and comparing it to open-times and duration.
 * Returns true if lid should be open
 */
bool handle_scheduler() {
    auto mintoday = current_minute();
    static bool should_be_open = false;

    // Only do heavy calculations if we did not already do it this minute
    static auto mintoday_last = 0;
    if (mintoday != mintoday_last) {
        mintoday_last = mintoday;

#ifdef SCHEDULER_DEBUG
        Serial.printf("== handle_scheduler(): Checking current minute %d\n", mintoday);
#endif

        bool open = false;
        for (auto entry : open_close) {
#ifdef SCHEDULER_DEBUG
            Serial.printf("Comparing: [%d] %02d:%02d - %3d minutes: ", i+1,
                        entry.open_hour, entry.open_minute,
                        entry.duration);
#endif
            auto open_minute = entry.open_hour*60 + entry.open_minute;
            auto open_duration = entry.duration;
            if ((mintoday >= open_minute && mintoday < (open_minute + open_duration)) ||
                (open_minute + open_duration >= 1440 && mintoday < open_minute + open_duration - 1440)) {
#ifdef SCHEDULER_DEBUG
                Serial.println("open");
#endif
                open = true;
            } else {
#ifdef SCHEDULER_DEBUG
                Serial.println("close");
#endif
            }
            should_be_open = open;
        }
    }
    return should_be_open;
}


/**
 * current_minute()
 * Calculates the current number of minutes since midnight
 */
uint16_t current_minute() {
    time_t now;
    time(&now);
    tm* localtm = localtime(&now);
    return localtm->tm_hour*60 + localtm->tm_min;
}


/**
 * output_schedules()
 * Outputs all configured schedules to serial port for debugging
 */
void output_schedules() {
    Serial.println("Configured open/close times:");
    for (auto entry : open_close) {
        Serial.printf("- %02d:%02d - %3d minutes\n",
                    entry.open_hour, entry.open_minute,
                    entry.duration);
    }
}
