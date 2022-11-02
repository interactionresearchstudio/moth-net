// ----------------------------------------------------------------------------
// Preferences initialization
// ----------------------------------------------------------------------------

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
    while (1) {
    }
  }
}
void initPrefs() {
  preferences.begin("data", false);
}

void deletePrefs() {
  preferences.clear();
}

bool isSavedMac(char* incomingMac) {
  String macAddresses = preferences.getString("macAddresses");
  Serial.println(macAddresses);
  // Deserialize the JSON document
  bool isMatch = false;
  if (macAddresses != NULL) {
    DeserializationError error = deserializeJson(macs, macAddresses);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }
    Serial.print("You have ");
    Serial.print(macs.size());
    Serial.println(" Mac Addresses.");
    for (byte i = 0; i < macs.size(); i++) {
      if (macs[i] == incomingMac) {
        isMatch = true;
      }
    }
  }
  macs.clear();
  return isMatch;
}

void addMac(char* incomingMac) {
  macs[macs.size()] = incomingMac;
  String macString = "";
  serializeJson(macs, macString);
  preferences.putString("macAddresses", macString);
}

void factoryReset() {
  Serial.println("factory reset!");
  deletePrefs();
  File file = SPIFFS.open("/json/connections.json", FILE_WRITE);
  SPIFFS.remove("/json/connections.json");
  SPIFFS.remove("/json/connections_backup.json");
  dynamicDoc = NULL;
  ESP.restart();

}

String getNetworkSSID() {
  return preferences.getString("wifiSSID");
}
String getNetworkPASS() {
  return preferences.getString("wifiPASS");
}


String getScanAsJsonString() {
  String jsonString;
  StaticJsonDocument<1000> jsonDoc;
  getScanAsJson(jsonDoc);
  serializeJson(jsonDoc[0], jsonString);
  jsonDoc.clear();
  return (jsonString);
}

void getScanAsJson(JsonDocument& jsonDoc) {
  JsonArray networks = jsonDoc.createNestedArray();

  int n = WiFi.scanNetworks();
  n = (n > MAX_NETWORKS_TO_SCAN) ? MAX_NETWORKS_TO_SCAN : n;

  //Array is ordered by signal strength - strongest first
  for (int i = 0; i < n; ++i) {
    String networkSSID = WiFi.SSID(i);
    if (networkSSID.length() <= SSID_MAX_LENGTH) {
      JsonObject network  = networks.createNestedObject();
      network["SSID"] = WiFi.SSID(i);
      //network["BSSID"] = WiFi.BSSIDstr(i);
      //network["RSSI"] = WiFi.RSSI(i);
    }
  }
}

//For DEBUG
void setNetwork() {
  //preferences.putString("wifiSSID", String(WIFI_SSID));
  // preferences.putString("wifiPASS", String(WIFI_PASS));
}

void setNetwork(String SSID, String PASS) {
  preferences.putString("wifiSSID", SSID);
  preferences.putString("wifiPASS", PASS);
  Serial.println("Network has been set!");
}
