#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include <AceButton.h>
using namespace ace_button;
#define LONG_PRESS 2000

#include <Preferences.h>
Preferences prefs;

//espnow msg
bool isReceivedMsg = false;
#define MSGTIMEOUT 480000
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

//wifi channel
int setWifi = 0;
uint8_t baseMac[6];
#define CHANNEL 1

//util
#define LED   2
int BTN_PIN =   0;
bool isPressed = false;
#define SENSOR_PIN 4

//Acebutton
AceButton buttonBuiltIn(BTN_PIN);
void handleButtonEvent(AceButton*, uint8_t, uint8_t);

//ESPNOW STRUCT
enum functionTypes {
  sensor,
  action,
  heartbeat,
  wifiChannel,
  blinking,
  heartbeatREQ
};

enum sensorTypes {
  cap_touch,
  simple_switch,
  cam_movement,
  radar,
  cam_photo,
  servo,
};

typedef struct struct_message {
  functionTypes function;
  sensorTypes sensors;
  uint8_t mac[6];
  int eventVal;
} struct_message;

struct_message msg;
struct_message outgoingReadings;
esp_now_peer_info_t peerInfo;

#define DEVICE_TYPE cap_touch

void setup() {
  pinMode(LED,         OUTPUT);
  pinMode(BTN_PIN,      INPUT);
  buttonBuiltIn.init(BTN_PIN, HIGH);
  if (DEVICE_TYPE == simple_switch) {
    pinMode(SENSOR_PIN, INPUT_PULLUP);
  } else if (DEVICE_TYPE == radar) {
    pinMode(SENSOR_PIN, INPUT);
  } else if (DEVICE_TYPE == cam_movement) {
    
  }
  ButtonConfig* buttonConfigBuiltIn = buttonBuiltIn.getButtonConfig();
  buttonConfigBuiltIn->setEventHandler(handleButtonEvent);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureClick);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfigBuiltIn->setLongPressDelay(LONG_PRESS);
  Serial.begin(115200); delay(500);
  initPrefs();
  initWiFi();
  //set random number for sending heartbeat
  randomSeed(analogRead(0));
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  Serial.println(WiFi.macAddress());
  initESPNOW();
}
void loop() {
  buttonBuiltIn.check();

  delay(30);
  checkChannel();
}
