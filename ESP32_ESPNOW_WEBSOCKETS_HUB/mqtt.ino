void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  String topicIn = String(topic);
  if (topicIn == (getAIOUser() + "/feeds/moth-hub-led")) {
    if ((char)payload[0] == '1') {
      blinkLed(50);
      sendBlink();
    }
  } else {
    checkTopic(topicIn, (char)payload[0]);
  }
  Serial.println();
}

void connectToMqtt() {
  String feedName;
  // Connect to WiFi
  // Connect to MQTT
  client.setServer("io.adafruit.com", 1883);
  client.setCallback(mqttCallback);
  if (getAIOUser() != "" && getAIOKey() != "") {
    if (client.connect("moth-net-cli", getAIOUser().c_str(), getAIOKey().c_str())) {
      Serial.println("Connected to MQTT server");
      digitalWrite(LED_PIN, HIGH);
      isConnectedAIO = true;
      //Test led feed
      feedName = getAIOUser() + String("/feeds/moth-hub-led");
      client.subscribe(feedName.c_str());
      //Default feed
      feedName = "test";
      subscribeToFeed(feedName);
      feedName = "";
      Serial.println("number of feeds");
      //get amount of feeds
      //subscribe
      // Open file for reading
      File file = SPIFFS.open("/json/connections.json", FILE_READ);
      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, file);
      if (error)
        Serial.println(F("Failed to read file, using default configuration"));
      file.close();
      //String feedOut = doc[index]["feed"].as<String>();
      Serial.println("Size of feeds");
      Serial.println(doc.size());
      file.close();
      //Subscribe to all feeds
      // String feedIn = getFeeds();
      // feeds.clear();
      // DeserializationError error = deserializeJson(feeds, feedIn);
      // Test if parsing succeeds.
      // if (error) {
      //   Serial.print(F("deserializeJson() failed: "));
      //   Serial.println(error.f_str());
      //  }
      Serial.print("You have ");
      Serial.print(doc.size());
      Serial.println(" feeds.");
      for (int i = 0; i < doc.size(); i++) {
        subscribeToFeed(doc[i]["feed"].as<String>());
      }
      doc.clear();
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
        delay(50);
        digitalWrite(LED_PIN, HIGH);
      }
    }
    else {
      Serial.println("Failed to connect to MQTT server!");
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
      delay(1000);
    }
  } else {
    Serial.println("AIO credentials have no been set yet, please input them into the captive portal");
  }
}

void subscribeToFeed(String feedName) {
  feedName = getAIOUser() + "/feeds/" + feedName;
  client.subscribe(feedName.c_str());
  Serial.print(feedName);
  Serial.println(" channel subscribed!");
}

void checkTopic(String topic, char valueOut) {
  //Check if anyone is subscribed to this channel
  File file = SPIFFS.open("/json/connections.json", FILE_READ);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  uint32_t messageToSend = 0;
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));
  file.close();
  Serial.println(topic);
  int macObject = 0;
  Serial.print(doc.size());
  Serial.println(" to check");
  String adafruitIo = getAIOUser() + "/feeds/";
  String docString = "";
  for (int i = 0; i < doc.size(); i++) {
    docString = adafruitIo + doc[i]["feed"].as<String>();
    Serial.println(docString);
    Serial.println(topic);
    if ((docString == topic) && (doc[i]["sensorType"].as<int>() == (int)cam_photo || doc[i]["sensorType"].as<int>() == (int)servo || doc[i]["sensorType"].as<int>() == (int)servo_continuous || doc[i]["sensorType"].as<int>() == (int)on_pin)) {
      Serial.println("Matched connection");
      macObject = i;
      // sendSensorTo(macObject, doc[i]["sensorType"].as<int>(), 0);
      bitWrite(messageToSend, i, 1);
      blinkLed(1);
    } else {
      //Serial.println("no matching connections");
    }
  }
  if (messageToSend > 0) {
    for (uint32_t i = 0; i < 32; i ++) {
      if (bitRead(messageToSend, i) == 1) {

        sendSensorTo(i, doc[i]["sensorType"].as<int>(), (int)valueOut);
        Serial.println("sending action message");
      }
    }
  }
  doc.clear();
}

void disconnectFromMqtt() {
  WiFi.mode(WIFI_STA);
}
