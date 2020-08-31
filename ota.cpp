/**
 * Using Arduinos OTA (Over the Air) Update to make developping a little bit easier.
 * Credit goes to the people who wrote the BasicOTA example code:
 * https://github.com/esp8266/Arduino/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "ota.h"

const char* ssid = STASSID;
const char* password = STAPSK;


/**
 * init_ota()
 * Expects to be called from `setup()` and prepares everything for OTA uploads.
 * Assumes `Serial` is configured.
 * Sets up WiFi.
 */
void init_ota() {
  Serial.println("== init_ota(): called");
  Serial.print("Connecting to WiFi... ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  uint8_t counter = 10;
  while (WiFi.waitForConnectResult() != WL_CONNECTED && counter > 0) {
    Serial.println("Not connected, retrying...");
    delay(1000);
    counter--;
  }
  
  ArduinoOTA.setHostname(HOSTNAME);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.print("Arduino OTA is ready and listening on IP: ");
  Serial.println(WiFi.localIP());
}

void handle_ota() {
  ArduinoOTA.handle();
}
