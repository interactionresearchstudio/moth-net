void createData(String feedname) {
  String serverPath = "http://io.adafruit.com/api/v2/vastltd/groups/moth-net/feeds/";
  serverPath = serverPath + feedname;
  serverPath = serverPath + "/data";
  WiFiClient client;
  HTTPClient httpPost;

  // Your Domain name with URL path or IP address with path
  httpPost.begin(client, serverPath);
  // Specify content-type header
  httpPost.addHeader("Content-Type", "application/json");
  httpPost.addHeader("X-AIO-Key", getAIOKey().c_str());
  // Data to send with HTTP POST
  // '{"value": 42, "lat": 23.1, "lon": "-73.3"}'
  String httpRequestData = "{\"value\": 42}";
  // Send HTTP POST request
  Serial.println(httpRequestData);
  int httpResponseCode = httpPost.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  // Free resources
  httpPost.end();

}

void createFeed(String feedname) {
  feedname.toLowerCase();
  String serverPath = "http://io.adafruit.com/api/v2/vastltd/groups/moth-net/feeds";

  WiFiClient client;
  HTTPClient httpPost;

  // Your Domain name with URL path or IP address with path
  httpPost.begin(client, serverPath);
  // Specify content-type header
  httpPost.addHeader("Content-Type", "application/json");
  httpPost.addHeader("X-AIO-Key", getAIOKey().c_str());
  // Data to send with HTTP POST
  //{"feed": {"name": "Feed Name"}}
  String httpRequestData = "{\"feed\": {\"name\": \"" + feedname;
  httpRequestData = httpRequestData + "\"}}";
  // Send HTTP POST request
  Serial.println(httpRequestData);
  int httpResponseCode = httpPost.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  // Free resources
  httpPost.end();

}
String getFeeds() {
  /*
  HTTPClient http;
  String payload = "";

  http.begin("https://io.adafruit.com/api/v2/vastltd/groups/moth-net/feeds"); //Specify the URL and certificate
  http.addHeader("X-AIO-Key", getAIOKey().c_str());
  http.setConnectTimeout(10000);
  int httpCode = http.GET();                                                  //Make the request

  if (httpCode > 0) { //Check for the returning code

    payload = http.getString();
    Serial.println(httpCode);
    //Serial.println(payload);
  } else {
    Serial.println("Error on HTTP request");
    Serial.println(httpCode);
  }

  http.end(); //Free the resources

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(feeds, payload);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }

  Serial.print("You have ");
  Serial.print(feeds.size());
  Serial.println(" feeds.");
  for (byte i = 0; i < feeds.size(); i++) {
    Serial.println(feeds[i]["name"].as<String>());
    feedsSimple[i]["name"] = "moth-net."+feeds[i]["name"].as<String>();
  }
  String sensor = "";
  serializeJson(feedsSimple, sensor);
  //Serial.println(sensor);
  feeds.clear();
  feedsSimple.clear();
  payload = "";
  return sensor;
  */
}

bool insertFeed(String feedName) {
  /*
  //needs to be lowercase as adafruit IO doesn't look for case.
  feedName.toLowerCase();
  feedName.trim();
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
  bool isNewFeed = true;
  String feedSaved;
  for (byte i = 0; i < feeds.size(); i++) {
    //Needs to lowercase.
    feedSaved = feeds[i]["name"].as<String>();
    feedSaved.toLowerCase();
    feedSaved.trim();
    Serial.println(feedSaved);
    if (feedSaved == feedName) {
      isNewFeed = false;
      break;
    }
  }
  if (isNewFeed == true) {
    Serial.print("Inserting feed ");
    Serial.print(feedName);
    Serial.println(" into feeds list");
    createFeed(feedName);
    return true;
  } else {
    Serial.println("We already have that feed");
    return false;
  }
  String sensor = "";
  serializeJson(feedsSimple, sensor);
  //Serial.println(sensor);
  return sensor;
  */
}

String getFeedByMac(String mac) {
  File file = SPIFFS.open("/json/connections.json", FILE_READ);
  DeserializationError error = deserializeJson(doc, file);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  file.close();
  Serial.print("You have ");
  Serial.print(doc.size());
  Serial.println(" feeds.");
  for (int i = 0; i < doc.size(); i++) {
    Serial.println(doc[i]["mac"].as<String>());
    Serial.println(mac);
    Serial.println();
    if (doc[i]["mac"].as<String>() == mac) {
      String out = doc[i]["feed"].as<String>();
      doc.clear();
      return out;
    }
  }
  doc.clear();
  return "";
}

String getAIOUser() {
  String out = preferences.getString("AIOUSER");
  Serial.println(out);
  return out;
}

String getAIOKey() {
  String out = preferences.getString("AIOKEY");
  out.trim();
  Serial.println(out);
  return out;
}


void setAIO(String USER, String KEY) {
  preferences.putString("AIOUSER", USER);
  preferences.putString("AIOKEY", KEY);
  Serial.println("AIO has been set!");
}
