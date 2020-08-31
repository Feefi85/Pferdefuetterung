#pragma once

#include <uRTCLib.h>

#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()

#include <TZ.h>

//#define TIME_DEBUG
#define MYTZ TZ_Europe_Zurich
#define NTP_POOL "ch.pool.ntp.org"

#define RTC_SDA 5     // I2C Bus for RTC module
#define RTC_SCL 4     // I2C Bus for RTC module
#define RTC_ADDR 0x68 // I2C Adress of RTC module

// Public methods
void init_time();
void handle_time();

// Internal callback handler
void time_to_rtc_cb();

// Helper
void rtc_to_tm(tm *buf);
void output_local();
void output_rtc();