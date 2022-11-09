bool isConnected = false;

void initWiFi() {
  WiFi.mode(WIFI_AP_STA);
  Serial.println("\n[*] Creating ESP32 AP");
  WIFI_AP_NAME = String(WIFI_SSID_AP) + "-" + generateID();
  WiFi.softAP(WIFI_AP_NAME.c_str(), WIFI_PASS_AP);
  Serial.print("[+] AP Created ");
  Serial.print(WIFI_AP_NAME);
  Serial.print(" with IP Gateway ");
  Serial.println(WiFi.softAPIP());

  if (getNetworkSSID() != "" && getNetworkPASS() != "") {
    String ssid = getNetworkSSID();
    String pass = getNetworkPASS();
    connectToRouter(ssid.c_str(), pass.c_str(), 60000);
  } else {
#ifdef NOUI
    connectToRouter(WIFI_SSID, WIFI_PASS, 60000);
#else
    Serial.println("No WiFi Router Credentials, Input in the AccessPoint to continue.");
#endif
  }

  Serial.print("WiFi Channel: ");
  Serial.println(WiFi.channel());
  setWifiChannel(WiFi.channel());

}

void connectToRouter(String SSID, String PASS, unsigned long timeOut) {
  WiFi.begin(SSID.c_str(), PASS.c_str());
  Serial.println("\n[*] Connecting to WiFi Network");

  unsigned long wifiTimeOut = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiTimeOut < timeOut))
  {
    Serial.print(".");
    delay(100);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi having problems connecting. Please check your wifi password");
    isConnected = false;
  } else {
    Serial.print("\n[+] Connected to the WiFi network with local IP : ");
    Serial.println(WiFi.localIP());
    isConnected = true;
  }
}

bool isConnectedToInternet() {
  if (WiFi.status() == WL_CONNECTED) {
    if(getWifiChannel() != WiFi.channel()){
      setWifiChannel(WiFi.channel());
    }
    isConnected = true;
    if (ledState == false) {
      ledState = true;
      digitalWrite(LED_PIN, HIGH);
    }
  } else {
    isConnected = false;
    if (ledState == true) {
      ledState = false;
      digitalWrite(LED_PIN, LOW);
    }
    if (millis() - prevConnectAttempt > reconnectInterval) {
      prevConnectAttempt = millis();
      WiFi.reconnect();
    }
  }
  return isConnected;
}

bool isConnectedToAIO() {
  return isConnectedAIO;
}
