void setEspNowChannel(int ch) {
  Serial.print("Channel: ");
  Serial.println(ch);
  setWifiChannel(ch);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

  if (esp_now_is_peer_exist(broadcastAddress)) {
    esp_now_del_peer(broadcastAddress);
  }

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = ch;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }


}

void initESPNOW() {


  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    //ESP.restart();
  }

  int ch = checkSavedChannel();
  setEspNowChannel(ch);

  esp_now_register_recv_cb(onDataReceive);
  esp_now_register_send_cb(OnDataSent);

}

int checkSavedChannel() {
  int chan = prefs.getInt("channel", 0);
  if (chan == 0) {
    Serial.println("no channel saved");
    return CHANNEL;
  } else {
    Serial.print("connecting to channel ");
    Serial.println(chan);
    return chan;
  }
}

void setSavedChannel(int chan) {
  prefs.putInt("channel", chan);
  Serial.println("saving channel to preferences");
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}


void checkChannel() {
  if (isReceivedMsg == false) {
    if (millis() > MSGTIMEOUT) {
      Serial.println("Never received heartbeat, setting back to channel 1");
      setSavedChannel(1);
      ESP.restart();
    }
  }
}

boolean array_cmp(uint8_t *a, uint8_t *b, int len_a, int len_b) {
  int n;
  // if their lengths are different, return false
  if (len_a != len_b) return false;
  // test each element to be the same. if not, return false
  for (n = 0; n < len_a; n++) if (a[n] != b[n]) return false;
  //ok, if we have not returned yet, they are equal :)
  return true;
}

// On data received
void onDataReceive(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  //Add CRC
  if (len == sizeof(msg)) {
    memcpy(&msg, incomingData, sizeof(msg));
    char macStr[18];
    snprintf(macStr, sizeof(macStr), " % 02x: % 02x: % 02x: % 02x: % 02x: % 02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print("Received from: ");
    Serial.println(macStr);
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Sensor type: ");
    Serial.println(msg.sensors);
    if (msg.function == wifiChannel) {
      if (msg.eventVal != getWifiChannel()) {
        setSavedChannel(msg.eventVal);
        setEspNowChannel(msg.eventVal);
        delay(2000);
        sendHeartbeat();
      }
    } else if (msg.function == blinking) {
      Serial.println("Blink!");
      blinkLed();
    } else if (msg.function == heartbeatREQ) {
      isReceivedMsg = true;
      //delay so they don't all send at the same time
      delay(random(100));
      sendHeartbeat();
    } else if (array_cmp(msg.mac, baseMac, 6, 6)) {
    }
  }
}


void sendHeartbeat() {
  // Send message via ESP-NOW
  outgoingReadings.function = heartbeat;
  outgoingReadings.sensors = DEVICE_TYPE;
  outgoingReadings.eventVal = 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

String success;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status: \t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success : )";
  }
  else {
    success = "Delivery Fail : (";
  }
}

int getWifiChannel() {
  return setWifi;
}

void setWifiChannel(int wifi) {
  setWifi = wifi;
}


void sendSensor(int valueIn) {
  // Send message via ESP-NOW
  outgoingReadings.function = sensor;
  outgoingReadings.sensors = DEVICE_TYPE;
  outgoingReadings.eventVal = valueIn;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
    blinkLed();
    delay(500);
  }
  else {
    Serial.println("Error sending the data");
  }
}
