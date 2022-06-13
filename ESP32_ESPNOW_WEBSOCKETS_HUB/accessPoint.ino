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

void initWebServer() {
  server.on("/", onRootRequest);
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}
