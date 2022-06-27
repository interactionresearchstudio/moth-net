#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
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

#define DEVICE_TYPE cam_movement


//util
#if(DEVICE_TYPE == cam_movement)
#define LED   4
bool isPressed = false;
#define SENSOR_PIN -1
#else
#define LED   2
int BTN_PIN =   0;
bool isPressed = false;
#define SENSOR_PIN 4
//Acebutton
AceButton buttonBuiltIn(BTN_PIN);
void handleButtonEvent(AceButton*, uint8_t, uint8_t);
#endif



#if(DEVICE_TYPE == cam_movement)
#include "esp_camera.h"
#include "camera_pins.h"
#define FRAME_SIZE FRAMESIZE_QVGA
#define WIDTH 320
#define HEIGHT 240
#define BLOCK_SIZE 10
#define W (WIDTH / BLOCK_SIZE)
#define H (HEIGHT / BLOCK_SIZE)
#define BLOCK_DIFF_THRESHOLD 0.2
#define IMAGE_DIFF_THRESHOLD 0.1
//#define DEBUG 1
uint16_t prev_frame[H][W] = { 0 };
uint16_t current_frame[H][W] = { 0 };
bool setup_camera(framesize_t);
bool capture_still();
bool motion_detect();
void update_frame();
void print_frame(uint16_t frame[H][W]);
#endif


void setup() {
  // pinMode(LED,         OUTPUT);
#if(DEVICE_TYPE != cam_movement)
  randomSeed(analogRead(0));
  pinMode(BTN_PIN,      INPUT);
  buttonBuiltIn.init(BTN_PIN, HIGH);
  ButtonConfig* buttonConfigBuiltIn = buttonBuiltIn.getButtonConfig();
  buttonConfigBuiltIn->setEventHandler(handleButtonEvent);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureClick);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfigBuiltIn->setLongPressDelay(LONG_PRESS);
#else
  randomSeed(analogRead(4));
#endif
  Serial.begin(115200); delay(500);
  initPrefs();
  initWiFi();
  //set random number for sending heartbeat
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  Serial.println(WiFi.macAddress());
  initESPNOW();

  if (DEVICE_TYPE == simple_switch) {
    pinMode(SENSOR_PIN, INPUT_PULLUP);
  } else if (DEVICE_TYPE == radar) {
    pinMode(SENSOR_PIN, INPUT);
  } else if (DEVICE_TYPE == cam_movement) {
    Serial.println(setup_camera(FRAME_SIZE) ? "OK" : "ERR INIT");
  }

}
void loop() {
#if(DEVICE_TYPE != cam_movement)
  buttonBuiltIn.check();
#endif
 // readSensor();
 checkCam();
  checkChannel();
}
