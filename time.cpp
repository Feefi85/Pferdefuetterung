/*
 * Methods for handling time and date. All things related to:
 * - RTC
 * - NTP
 * - local timekeeping
 * 
 * Inspired by:
 * - https://github.com/tht/ValentineBox/blob/master/src/main.cpp
 * - https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/NTP-TZ-DST/NTP-TZ-DST.ino
 * - https://github.com/Naguissa/uRTCLib/blob/master/examples/uRTCLib_example/uRTCLib_example.ino
 */
#include "time.h"

// uRTCLib rtc, I2C Address;
uRTCLib rtc(RTC_ADDR);


/**
 * init_time()
 * Reads RTC and sets internal clock correspondingly. Also configures `configTime(…)` (SNTP)
 * to sync to NTP if possible. Registers callback to get notified if SNTP updates local time
 * to make sure RTC is also updated.
 */
void init_time() {
  Serial.println("== init_time(): called");
  // Configure RTC and fetch time
  Wire.begin(RTC_SDA, RTC_SCL); // D2 and D1 on ESP8266

  // Fill `tm` struct with RTC data and use `settimeofday(…)` to update internal clock
  tm rtc_time;
  rtc_to_tm(&rtc_time);
  time_t rtc_now = mktime(&rtc_time);
  timeval rtc = {rtc_now, 0};
  settimeofday(&rtc, nullptr);

  output_rtc();

  // Register callback and set up SNTP
  settimeofday_cb(time_to_rtc_cb);
  configTime(MYTZ, NTP_POOL);
}


/**
 * time_to_rtc_cb()
 * Callback registered in `init_time()`. Gets called every time `settimeofday(…)` is used.
 * Writes time down to RTC.
 */
void time_to_rtc_cb() {
  Serial.println("-- settimeofday(): called - possibly from SNTP");

  // Read internal clock and parse into `tm`.
  timeval tv;
  gettimeofday(&tv, nullptr);
  time_t now = tv.tv_sec;
  tm* gmtm = gmtime(&now);

  // Write to RTC, shift some values as RTC and `tm` do have other base values
  rtc.set(gmtm->tm_sec, gmtm->tm_min, gmtm->tm_hour, gmtm->tm_wday+1, gmtm->tm_mday, gmtm->tm_mon+1, 1900 + gmtm->tm_year - 2000);
}


/**
 * rtc_to_tm(…)
 * Reads date from RTC and put result in given tm struct.
 */
void rtc_to_tm(tm *buf) {
  rtc.refresh();

  buf->tm_sec  = rtc.second();
  buf->tm_min  = rtc.minute();
  buf->tm_hour = rtc.hour();
  buf->tm_wday = rtc.dayOfWeek() - 1;
  buf->tm_mday = rtc.day();
  buf->tm_mon  = rtc.month() - 1;
  buf->tm_year = rtc.year() + 2000 - 1900;
}


/**
 * output_rtc()
 * Outputs current RTC date/time to serial for debugging purposes.
 */
void output_rtc() {
  tm rtctm;
  rtc_to_tm(&rtctm);
	Serial.printf("The RTC date and time is:   %s", asctime(&rtctm));
}


/**
 * output_local()
 * Outputs current local date/time to serial for debugging purposes.
 */
void output_local() {
  time_t now;
  time(&now);
  tm* localtm = localtime(&now);

  Serial.printf("The local date and time is: %s", asctime(localtm));
}


/**
 * handle_time()
 * Needs to be called often.
 * Currently only debug output.
 */
void handle_time() {

#ifdef TIME_DEBUG
  static uint32_t last_rtc = 0;
  if (millis() - last_rtc > 1000) {
    last_rtc = millis();

    output_rtc();
    output_local();
    Serial.println();
  }
#endif

}