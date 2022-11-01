//To connect a new sensor to the correct channel, turn on the sensor, wait 5 seconds, then reset your hub

#include <Arduino.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <AceButton.h>
using namespace ace_button;
#include "config.h"

/*
   In config.h so not shared with github
  String AIOUSER = "NAME";
  String AIOKEY = "aio_PASS";
  const char *WIFI_SSID = "NAME";
  const char *WIFI_PASS = "PASS";

*/


bool isSendingData = false;
byte numConnectedDevices = 0;


#define LED_PIN   2
int BTN_PIN = 0;

#define MAX_NETWORKS_TO_SCAN 5
#define SSID_MAX_LENGTH 31


//ESP-Now settings
enum sensorTypes {
  cap_touch,
  simple_switch,
  cam_movement,
  radar,
  cam_photo,
  servo,
  servo_continuous,
  on_pin,
  hallEffect
};

enum functionTypes {
  sensor,
  action,
  heartbeat,
  wifiChannel,
  blinking,
  heartbeatREQ
};

//Structure example to send data
typedef struct struct_message {
  functionTypes function;
  sensorTypes sensors;
  uint8_t mac[6];
  int eventVal;
} struct_message;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//This needs to become a get/set function and not global
uint8_t macToSend[6];

//WiFi access point credentials
char *WIFI_SSID_AP = "MOTHNET";
const char *WIFI_PASS_AP = "password";
String WIFI_AP_NAME;

//MQTT
WiFiClient espClient;
PubSubClient client(espClient);
bool wifiScanSend = false;
bool sensorScanSend = false;
bool sendFeeds = false;
bool connectedStatusSend = false;
bool nameSend = false;

// JSON Docs
StaticJsonDocument<8000> feeds;
DynamicJsonDocument feedsSimple(2000);
DynamicJsonDocument macs(1000);
StaticJsonDocument<1000> doc;
DynamicJsonDocument connectedMacs(1000);
JsonArray macsOnline;

//Preferences
Preferences preferences;

//Acebutton
AceButton buttonBuiltIn(BTN_PIN);

//Websockets
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//AIO
bool isConnectedAIO = false;


void setup() {

  Serial.begin(115200); delay(500);
  setupPins();
  initSPIFFS();
  initPrefs();
  Serial.println(loadJSON());
  initConnectedMacJson();
  //setNetwork(); //Only use when no UI to input credentials

  //We need to do wifi/espnow/wifi initialisation to get the wifi channel to be able to set sensors to the correct WiFi channel.
  initWiFi();
  initESPNOW();
  initWiFi();
  if (isConnectedToInternet()) {
    connectToMqtt();
  }
  initWebSocket();
  initWebServer();

}
bool ledState = false;

void loop() {
  if (isConnectedToInternet()) {
    if (ledState == false) {
      ledState = true;
      digitalWrite(LED_PIN, HIGH);
    }
    client.loop();
  } else {
    if (ledState == true) {
      ledState = false;
      digitalWrite(LED_PIN, LOW);
    }
  }
  ws.cleanupClients();
  buttonBuiltIn.check();
  checkWebsocketRequests();
  heartBeatHandler();
}
