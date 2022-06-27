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
    checkTopic(topicIn);
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
      isConnectedAIO = true;
      //Test led feed
      feedName = getAIOUser() + String("/feeds/moth-hub-led");
      client.subscribe(feedName.c_str());
      //Default feed
      feedName = "moth-net.newsensor";
      subscribeToFeed(feedName);
      feedName = "";
      //Subscribe to all feeds
      String feedIn = getFeeds();
      feeds.clear();
      DeserializationError error = deserializeJson(feeds, feedIn);
      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
      }
      Serial.print("You have ");
      Serial.print(feeds.size());
      Serial.println(" feeds.");
      for (int i = 0; i < feeds.size(); i++) {
        subscribeToFeed(feeds[i]["name"]);
      }
      feeds.clear();
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
        delay(50);
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

void checkTopic(String topic) {
  //Check if anyone is subscribed to this channel
  File file = SPIFFS.open("/json/connections.json", FILE_READ);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));
  file.close();
  Serial.println(topic);
  int macObject = doc.size();
  for (int i = 0; i < doc.size(); i++) {
    String docString =  getAIOUser() + "/feeds/";
    docString = docString + doc[i]["feed"].as<String>();
    if ((docString == topic) && (doc[i]["sensorType"].as<int>() == (int)cam_photo || doc[i]["sensorType"].as<int>() == (int)servo || doc[i]["sensorType"].as<int>() == (int)servo_continuous || doc[i]["sensorType"].as<int>() == (int)on_pin)) {
      Serial.println("Matched connection");
      macObject = i;
      sendSensorTo(macObject, doc[i]["sensorType"].as<int>(), 0);
      blinkLed(100);
    } else {
      //Serial.println("no matching connections");
    }
  }
  doc.clear();
}

void disconnectFromMqtt() {
  WiFi.mode(WIFI_STA);
}
