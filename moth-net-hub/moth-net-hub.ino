// Adafruit IO Analog In Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-analog-input
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/



// set up the 'analog' feed
AdafruitIO_Feed *freader1 = io.feed("freader-hucknall-1");

#include <esp_now.h>
#include <WiFi.h>

uint8_t globalMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
char rfidStr[18];
typedef struct message_struct {
  uint8_t rfid[4];
} message_struct;

message_struct msg;

void setup() {

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }

  esp_now_register_recv_cb(onDataReceive);


}

double lat = 42.331427;
double lon = -83.045754;
double ele = 0;
bool toSend = false;
char rfid[18];

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  if (toSend) {
    toSend = false;
    
      // connect to io.adafruit.com
      Serial.print("Connecting to Adafruit IO");
      io.connect();

      // wait for a connection
      while (io.status() < AIO_CONNECTED) {
      Serial.print(".");
      delay(500);
      }

      // we are connected
      Serial.println();
      Serial.println(io.statusText());
      
      freader1->save(rfidStr, lat, lon, ele);

      delay(4000);
      WiFi.mode(WIFI_STA);
     // WiFi.disconnect();
      if (esp_now_init() == ESP_OK) {
      Serial.println("ESPNow Init Success");
      }
      else {
      Serial.println("ESPNow Init Failed");
      ESP.restart();
      }
      io.run();
    
  }

  //  String tag = "01D5F3B2";
  //  // return if the value hasn't changed
  //
  //  // save the current state to the analog feed
  //  Serial.print("sending -> ");
  //  Serial.println(tag);
  //  freader1->save(tag, lat, lon, ele);
  //
  delay(10);
}
// On data received
void onDataReceive(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&msg, incomingData, sizeof(msg));
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.print("Received from: ");
  Serial.println(macStr);
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Data: ");
  
  snprintf(rfidStr, sizeof(rfidStr), "%02x%02x%02x%02x", msg.rfid[0], msg.rfid[1], msg.rfid[2], msg.rfid[3]);
  //Serial.println(rfidStr);
  Serial.print("sending...");
  toSend = true;
}