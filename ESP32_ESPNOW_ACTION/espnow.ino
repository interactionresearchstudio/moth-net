void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
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

void setEspNowChannel(int ch) {
  Serial.print("Channel: ");
  Serial.println(ch);
  setWifiChannel(ch);

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


int getWifiChannel() {
  return setWifi;
}

void setWifiChannel(int wifi) {
  setWifi = wifi;
}

String success;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  }
  else {
    success = "Delivery Fail :(";
  }
}

void checkChannel() {
  if (millis() - heartbeatTimeout > MSGTIMEOUT) {
    Serial.println("Never received heartbeat, scanning channels");
    heartbeatTimeout = millis();
    scanChannels();
    //ESP.restart();
  }
  scanChannelHandler();
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
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print("Received from: ");
    Serial.println(macStr);
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Sensor type: ");
    Serial.println(msg.sensors);
    if (msg.function == wifiChannel) {
      Serial.println("wifi channel setting");
      if (msg.eventVal != currWifiChannel) {
        Serial.println("got new wifi channel");
        setEspNowChannel(msg.eventVal);
        setSavedChannel(msg.eventVal);
        setScanComplete();
        delay(random(100));
        sendHeartbeat();
      } else {
        Serial.println("wifi is already the current wifi channel");
        setEspNowChannel(msg.eventVal);
        setSavedChannel(msg.eventVal);
        setScanComplete();
        delay(random(100));
        sendHeartbeat();
      }
    } else if (msg.function == heartbeatREQ) {
      heartbeatTimeout = millis();
      isReceivedMsg = true;
      //delay so they don't all send at the same time
      delay(random(100));
      sendHeartbeat();
    } else if (msg.function == blinking) {
      Serial.println("Blink!");
      blinkLed();
    }
    if (msg.function == action) {
      Serial.println("it's an action");
      if (array_cmp(msg.mac, baseMac, 6, 6)) {
        Serial.println("sent to my mac address");
        performAction(msg.eventVal);
      } else {
        Serial.println("for another sensor");
      }
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

void sendSensor() {
  // Send message via ESP-NOW

  outgoingReadings.function = action;
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

void scanChannels() {
  readyToScanChannels = true;
}

void setScanComplete() {
  readyToScanChannels = false;
  isWaitingforScanAck = false;
  scanChannel = 1;
  currWifiChannel = getWifiChannel();
}
void scanChannelHandler() {
  if (readyToScanChannels == true) {
    if (isWaitingforScanAck == false) {
      if (scanChannel > 13) {
        readyToScanChannels = false;
        scanChannel = 1;
      } else {
        isWaitingforScanAck = true;
        setEspNowChannel(scanChannel);
        outgoingReadings.function = wifiChannelREQ;
        outgoingReadings.sensors = DEVICE_TYPE;
        outgoingReadings.eventVal = scanChannel;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
        Serial.print("Sending wifi on channel ");
        Serial.println(scanChannel);
        scanAckTime = millis();
      }
    }
    if (isWaitingforScanAck == true) {
      if (millis() - scanAckTime > scanWaitTime) {
        isWaitingforScanAck = false;
        scanChannel++;
      }
    }
  }
}
