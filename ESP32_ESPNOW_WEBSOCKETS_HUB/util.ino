#define LONG_PRESS 5000

void handleButtonEvent(AceButton*, uint8_t, uint8_t);

void setupPins() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);

  ButtonConfig* buttonConfigBuiltIn = buttonBuiltIn.getButtonConfig();
  buttonConfigBuiltIn->setEventHandler(handleButtonEvent);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureClick);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfigBuiltIn->setLongPressDelay(LONG_PRESS);
}

void blinkLed(int delayTime) {
  digitalWrite(LED_PIN, 1);
  delay(delayTime);
  digitalWrite(LED_PIN, 0);
}

// button functions
void handleButtonEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (button->getPin()) {
    case 0:
      switch (eventType) {
        case AceButton::kEventPressed:
          getFeeds();
          break;
        case AceButton::kEventReleased:
          break;
        case AceButton::kEventLongPressed:
          factoryReset();
          break;
        case AceButton::kEventRepeatPressed:
          break;
      }
      break;
  }
}
// Loads the configuration from a file
String loadJSON() {
  String out = "";
  // Open file for reading
  File file = SPIFFS.open("connections.json", FILE_READ);
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println(F("Failed to read JSON file, using default configuration"));
  } else {
    //Serial.println(doc.as<String>());
    Serial.println(doc.size());
    out = doc.as<String>();
  }
  file.close();
  if (out == "") {
    doc.clear();
    File file2 = SPIFFS.open("connections_backup.json", FILE_READ);
    DeserializationError error = deserializeJson(doc, file2);
    if (error) {
      Serial.println(F("Failed to read backup JSON file, using default configuration"));
    } else {
      Serial.println(doc.size());
      out = doc.as<String>();
      Serial.println(out);
    }
    file2.close();
    if (out == "") {
      Serial.println("back up file also empty");
      File file = SPIFFS.open("connections.json", FILE_WRITE);

      if (!file) {
        Serial.println("There was an error opening the file for writing");
      }

      if (file.print("[]")) {
        Serial.println("File was written");
      } else {
        Serial.println("File write failed");
      }
      file.close();
      file = SPIFFS.open("connections_backup.json", FILE_WRITE);

      if (!file) {
        Serial.println("There was an error opening the file for writing");
      }

      if (file.print("[]")) {
        Serial.println("File was written");
      } else {
        Serial.println("File write failed");
      }
      file.close();

    } else {
      SPIFFS.remove("connections_backup.json");
      SPIFFS.remove("connections.json");
      SPIFFS.rename("connections_backup.json", "connections.json");
      Serial.println("imported backup file");
    }
  }
  doc.clear();
  // Close the file (Curiously, File's destructor doesn't close the file)
  //file.close();
  return out;
}

void updateJson(const char* jsonIn) {
  SPIFFS.remove("connections_backup.json");
  SPIFFS.rename("connections.json", "connections_backup.json");
  SPIFFS.remove("connections.json");
  File file = SPIFFS.open("connections.json", FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  file.print(jsonIn);
  // Close the file
  file.close();
}

void saveJSON(char* macStr, sensorTypes sensor) {
  bool updateJson = false;
  File file = SPIFFS.open("connections.json", FILE_READ);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));
  file.close();

  // Open file for writing
  int macObject = doc.size();
  for (int i = 0; i < doc.size(); i++) {
    if (doc[i]["mac"] == macStr) {
      macObject = i;
      break;
    }
  }
  if (macObject == doc.size()) {
    updateJson = true;
    JsonObject obj = doc.createNestedObject();
    // Set the values in the document
    Serial.println("no matches in json file found");
    obj["name"] = getNamefromSensor(sensor);
    obj["mac"] = macStr;
    obj["feed"] = "moth-net.newsensor";
    obj["sensorType"] = (int)sensor;
    obj["value"] = "99";
    obj["connected"] = true;
  } else {
    if (doc[macObject]["sensorType"].as<int>() == (int)sensor) {
      Serial.println("found a match in the json file, no need to update");
      updateJson = false;
    } else {
      updateJson = true;
      Serial.println("updating json");
      doc[macObject]["name"] = getNamefromSensor(sensor);
      doc[macObject]["mac"] = macStr;
      doc[macObject]["feed"] = "moth-net.newsensor";
      doc[macObject]["sensorType"] = (int)sensor;
      doc[macObject]["value"] = "99";
      doc[macObject]["connected"] = true;
    }
  }
  if (updateJson) {
    SPIFFS.remove("connections_backup.json");
    SPIFFS.rename("onnections.json", "connections_backup.json");
    Serial.println("copied over to backup");
    SPIFFS.remove("/connections.json");
    File file2 = SPIFFS.open("connections.json", FILE_WRITE);
    if (!file2) {
      Serial.println(F("Failed to create file"));
      return;
    }

    // Serialize JSON to file
    if (serializeJson(doc, file2) == 0) {
      Serial.println(F("Failed to write to file"));
    }
    // Close the file
    file2.close();
  }
  doc.clear();
}

String getNamefromSensor(sensorTypes sensor) {
  String result = "";
  switch (sensor) {
    case cap_touch:
      result = "Capacitive Touch";
      break;
    case simple_switch:
      result = "Simple Switch";
      break;
    case cam_movement:
      result = "Cam Movement";
      break;
    case radar:
      result = "Radar";
      break;
    case cam_photo:
      result = "Photo";
      break;
    case servo:
      result = "Servo";
      break;
  }

  return result;
}

void setAllToUnconnected() {
  File file = SPIFFS.open("connections.json", FILE_READ);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));
  file.close();

  // Open file for writing
  int macObject = doc.size();
  for (int i = 0; i < doc.size(); i++) {
    doc[i]["connected"] = false;
  }
  SPIFFS.remove("connections_backup.json");
  SPIFFS.rename("cconnections.json", "connections_backup.json");
  Serial.println("copied over to backup");
  SPIFFS.remove("connections.json");
  File file2 = SPIFFS.open("connections.json", FILE_WRITE);
  if (!file2) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Serialize JSON to file
  if (serializeJson(doc, file2) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  // Close the file
  file2.close();
  doc.clear();
}

void setDeviceToConnected(char* macStr) {
  File file = SPIFFS.open("connections.json", FILE_READ);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));
  file.close();

  // Open file for writing
  int macObject = doc.size();
  for (int i = 0; i < doc.size(); i++) {
    if (doc[i]["mac"] == macStr) {
      doc[i]["connected"] = true;
      break;
    }
  }
  SPIFFS.remove("connections_backup.json");
  SPIFFS.rename("connections.json", "connections_backup.json");
  Serial.println("copied over to backup");
  SPIFFS.remove("connections.json");
  File file2 = SPIFFS.open("connections.json", FILE_WRITE);
  if (!file2) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Serialize JSON to file
  if (serializeJson(doc, file2) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  // Close the file
  file2.close();
  doc.clear();
}

void getMacArray(int index) {
  //populates "macToSend" with the uint8_t array of the mac address

  // Open file for reading
  File file = SPIFFS.open("connections.json", FILE_READ);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));
  String macIn = doc[index]["mac"].as<String>();
  char* ptr; //start and end pointer for strtol
  macToSend[0] = strtol(macIn.c_str(), &ptr, HEX );
  for ( uint8_t i = 1; i < 6; i++ )
  {
    macToSend[i] = strtol(ptr + 1, &ptr, HEX );
  }
  Serial.print(macToSend[0], HEX);
  for ( uint8_t i = 1; i < 6; i++)
  {
    if (macToSend[i] < 16) {
      Serial.print(':');
      Serial.print("0");
      Serial.print( macToSend[i], HEX);
    } else {
      Serial.print(':');
      Serial.print( macToSend[i], HEX);
    }
  }
  Serial.println();
  doc.clear();
}


//Utility for comparing two arrays
boolean array_cmp(uint8_t *a, uint8_t *b, int len_a, int len_b) {
  int n;
  // if their lengths are different, return false
  if (len_a != len_b) return false;
  // test each element to be the same. if not, return false
  for (n = 0; n < len_a; n++) if (a[n] != b[n]) return false;
  //ok, if we have not returned yet, they are equal :)
  return true;
}

String generateID() {
  //https://github.com/espressif/arduino-esp32/issues/3859#issuecomment-689171490
  uint64_t chipID = ESP.getEfuseMac();
  uint32_t low = chipID % 0xFFFFFFFF;
  uint32_t high = (chipID >> 32) % 0xFFFFFFFF;
  String out = String(low);
  return  out;
}
