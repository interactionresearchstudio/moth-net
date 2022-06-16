/*to do
   -replace json of devices I get
   -better way of looking for json and strings
*/


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
   status: {"aio_connected": true, "wifi_connected": true}
   networks:  {"SSID":"VASTNET"},{"SSID":"NU Simply Web"},{"SSID":"WiFi Guest"},{"SSID":"eduroam"},{"SSID":"NU IOT"}
   feeds: {"feedName":"djfdsj"},{"name":"djshdf"},{"name":"dffsds"}
   devices: {"name":"New Device","mac":"ac:67:b2:2b:19:2c","feed":"moth-net.newsensor","sensorType":5,"value":"99","connected":true},{"name":"New Device","mac":"f0:08:d1:d7:67:dc","feed":"moth-net.newsensor","sensorType":0,"value":"99"}
*/

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String in = String((char*)data);
    Serial.println(in);
    if (len > 2) {
      if (in.indexOf("{\"") >= 0 && in.indexOf("SSID") >= 0 ) {
        Serial.println("it's networks");
        const uint8_t size = JSON_OBJECT_SIZE(2);
        StaticJsonDocument<size> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(err.c_str());
          return;
        }
        const char *SSIDIN = json["SSID"];
        Serial.println(SSIDIN);
        const char *PASSIN = json["PASS"];
        Serial.println(PASSIN);
        setNetwork(SSIDIN, PASSIN);
        connectToRouter(String(SSIDIN), String(PASSIN), 60000);
        setWifiChannel(WiFi.channel());
        connectedStatusSend = true;
        json.clear();
      } else if (in.indexOf("{\"") >= 0 && in.indexOf("aio_user") >= 0 ) {
        Serial.println("it's AIO");
        const uint8_t size = JSON_OBJECT_SIZE(2);
        StaticJsonDocument<size> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(err.c_str());
          return;
        }
        const char *aio_user = json["aio_user"];

        Serial.println(aio_user);
        const char *aio_key = json["aio_key"];
        Serial.println(aio_key);
        setAIO(String(aio_user), String(aio_key));
        json.clear();
        if (isConnectedToAIO() == false) {
          connectToMqtt();
        }
        connectedStatusSend = true;
      } else if (in.indexOf("{\"") >= 0 && in.indexOf("mac") >= 0 ) {
        Serial.println("it's device list");
        in = in.substring(0, in.indexOf("}]"));
        Serial.println("in");
        updateJson(in.c_str());
      } else if (in.indexOf("networks") >= 0) {
        // send network scan
        blinkLed(50);
        wifiScanSend = true;
      } else if (in.indexOf("status") >= 0) {
        // send connected states
        blinkLed(50);
        connectedStatusSend = true;
      } else if (in.indexOf("feeds") >= 0) {
        // send feeds
        blinkLed(50);
        sendFeeds = true;
      } else if (in.indexOf("devices") >= 0) {
        //send devices
        blinkLed(50);
        sensorScanSend = true;
      } else if (in.indexOf("devices") >= 0) {
        blinkLed(50);
        nameSend = true;

      }
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
  //setAllToUnconnected();
  updateWithConnectedMacs();
  
  String scan = loadJSON();
  char buf[2000];
  scan.toCharArray(buf, scan.length() + 1);
  ws.textAll(buf, scan.length());
  Serial.println(scan);
}

void sendConnectedStatus() {
  char buf[200];
  String out;
  if (isConnectedToInternet() && isConnectedToAIO()) {
    out = "{\"aio_connected\": true, \"wifi_connected\": true}";
  } else if (isConnectedToInternet() && isConnectedToAIO() == false) {
    out = "{\"aio_connected\": false, \"wifi_connected\": true}";
  } else if (isConnectedToInternet() == false && isConnectedToAIO()) {
    out = "{\"aio_connected\": true, \"wifi_connected\": false}";
  } else if (isConnectedToInternet() == false && isConnectedToAIO() == false) {
    out = "{\"aio_connected\": false, \"wifi_connected\": false}";
  }
  out.toCharArray(buf, out.length() + 1);
  ws.textAll(buf, out.length());
  Serial.println(out);
}


void sendFeedsScan() {
  char buf[2000];
  String scan = getFeeds();
  scan.toCharArray(buf, scan.length() + 1);
  ws.textAll(buf, scan.length());
  Serial.println(scan);
  delete buf;
}


void sendName() {
  char buf[50];
  WIFI_AP_NAME.toCharArray(buf, WIFI_AP_NAME.length() + 1);
  ws.textAll(buf, WIFI_AP_NAME.length());
  Serial.println("sent wifi ssid");
  delete buf;
}


//Functions triggered by buttons on webpage
void checkWebsocketRequests() {
  isWiFiScanReady();
  isSensorScanReady();
  isFeedsReady();
  isConnectedStatusReady();
  isNameSendReady();
}

void isFeedsReady() {
  if (sendFeeds == true) {
    sendFeedsScan();
    sendFeeds = false;
  }
}

void isConnectedStatusReady() {
  if (connectedStatusSend == true) {
    sendConnectedStatus();
    connectedStatusSend = false;
  }
}

void isNameSendReady() {
  if (nameSend == true) {
    sendName();
    nameSend = false;
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
