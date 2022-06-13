
void notifyClients() {
  String out = getFeeds();
  char buf[1000];
  out.toCharArray(buf, out.length() + 1);
  ws.textAll(buf, out.length());
  Serial.println(out);
}

/*UI Sends
    "networks", "devices", "feeds", "status"
  {"SSID":"FJFJJF","PASS": "FDKJFKJDFJK"};
  {"aio_user":"FJFJJF","aio_key": "FDKJFKJDFJK"};
  //connections.json from UI to replace
*/
/*ESP sends
    {"aio_connected": true, "wifi_connected": true}
    {"SSID":"VASTNET"},{"SSID":"NU Simply Web"},{"SSID":"WiFi Guest"},{"SSID":"eduroam"},{"SSID":"NU IOT"}
    {"name":"djfdsj"},{"name":"djshdf"},{"name":"dffsds"}
    {"name":"New Device","mac":"ac:67:b2:2b:19:2c","feed":"moth-net.newsensor","sensorType":5,"value":"99","connected":true},{"name":"New Device","mac":"f0:08:d1:d7:67:dc","feed":"moth-net.newsensor","sensorType":0,"value":"99"}
*/

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

    const uint8_t size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<size> json;
    DeserializationError err = deserializeJson(json, data);
    if (err) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
      return;
    }
    Serial.println((char*)data);
    const char *action = json["action"];
    int testIn = json["test"];
    Serial.println(testIn);
    if (strcmp(action, "toggle") == 0) {
      sendBlink();
      blinkLed(50);
      wifiScanSend = true;
    } else  if (strcmp(action, "toggle2") == 0) {
      sendBlink();
      blinkLed(50);
      sensorScanSend = true;
    } else  if (strcmp(action, "toggle3") == 0) {
      sendBlink();
      blinkLed(50);
      sendFeeds = true;
    }

  }
}

void onEvent(AsyncWebSocket       * server,
             AsyncWebSocketClient * client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len) {

  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


void sendWiFiScan() {
  String scan = getScanAsJsonString();
  char buf[1000];
  scan.toCharArray(buf, scan.length() + 1);
  ws.textAll(buf, scan.length());
  Serial.println(scan);
}

void sendSensorScan() {
  String scan = loadJSON();
  char buf[2000];
  scan.toCharArray(buf, scan.length() + 1);
  ws.textAll(buf, scan.length());
  Serial.println(scan);
}

void sendFeedsScan() {
  char buf[2000];
  String scan = getFeeds();
  scan.toCharArray(buf, scan.length() + 1);
  ws.textAll(buf, scan.length());
  Serial.println(scan);
}


//Functions triggered by buttons on webpage
void checkWebsocketRequests() {
  isWiFiScanReady();
  isSensorScanReady();
  isFeedsReady();
}

void isFeedsReady() {
  if (sendFeeds == true) {
    sendFeedsScan();
    sendFeeds = false;
  }
}

void isSensorScanReady() {
  if (sensorScanSend == true) {
    sendSensorScan();
    sensorScanSend = false;
  }
}

void isWiFiScanReady() {
  if (wifiScanSend == true) {
    sendWiFiScan();
    wifiScanSend = false;
  }
}
