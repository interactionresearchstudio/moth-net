// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------

String processor(const String & var) {
  //return String(var == "STATE" && led.on ? "on" : "off");
  return ("STATE");
}

void onRootRequest(AsyncWebServerRequest * request) {
  request->send(SPIFFS, "/index.html", "text/html", false, processor);
}
void jsRequest(AsyncWebServerRequest * request) {
  //request->send(SPIFFS, "static/js/main.2a33a2fb.js", "text/javascript", true, processor);
  Serial.println("got get js");
}

void cssRequest(AsyncWebServerRequest * request) {
  //  request->send(SPIFFS, "static/css/main.e7720ab7.css.map", "text/javascript", true, processor);
  Serial.println("got get");
}



void initWebServer() {
  listDir(SPIFFS, "/", 0);

  server.on("/", onRootRequest);
  //server.on("/static/js/main.2a33a2fb.js", HTTP_GET, jsRequest);
  // server.on("/static/css/main.e7720ab7.css.map", HTTP_GET, cssRequest);

  server.serveStatic("/", SPIFFS, "/");
  server.onNotFound(notFound);
  //server.onNotFound([](AsyncWebServerRequest * request) {
  //  Serial.printf("Not found: %s!\r\n", request->url().c_str());
  //  request->send(404);
  //});

  server.begin();
}

void notFound(AsyncWebServerRequest * request) {
  // here comes some mambo-jambo to extract the filename from request->url()
  //int fnsstart = request->url().lastIndexOf('/');
  String fn = request->url();
  Serial.println(fn);
  // ... and finally
  request->send(SPIFFS, fn, "text/javascript", false);
}


void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.print (file.name());
      time_t t = file.getLastWrite();
      struct tm * tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, ( tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size());
      time_t t = file.getLastWrite();
      struct tm * tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, ( tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
    }
    file = root.openNextFile();
  }
}
