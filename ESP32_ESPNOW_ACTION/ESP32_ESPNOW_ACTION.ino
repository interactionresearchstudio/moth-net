//#define CAM_PHOTO_DEVICE
//#define SERVO_DEVICE
#define SERVO_CONTINUOUS_DEVICE
//#define ON_PIN_DEVICE

#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <AceButton.h>
using namespace ace_button;
#define LONG_PRESS 4000
#include <Preferences.h>
Preferences prefs;
// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------
bool isReceivedMsg = false;
#define MSGTIMEOUT 480000
int setWifi = 0;
uint8_t baseMac[6];

#if defined(CAM_PHOTO_DEVICE)
#define LED_PIN -1
#define USER_PIN -1
int BTN_PIN =   -1;
#else
#define LED_PIN   2
#define USER_PIN 4
int BTN_PIN =   0;
//Acebutton
AceButton buttonBuiltIn(BTN_PIN);
void handleButtonEvent(AceButton*, uint8_t, uint8_t);
#endif
//SET BASED OFF HUB
#define CHANNEL 1


// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ----------------------------------------------------------------------------
// Definition of the ESPNOW struct
// ----------------------------------------------------------------------------

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

struct_message msg;
struct_message outgoingReadings;
esp_now_peer_info_t peerInfo;

#if defined(SERVO_DEVICE)
#include <ESP32Servo.h>
Servo myservo;
#define DEVICE_TYPE servo
#elif defined(CAM_PHOTO_DEVICE)
#define DEVICE_TYPE cam_photo
#elif defined(SERVO_CONTINUOUS_DEVICE)
#include <ESP32Servo.h>
Servo myservo;
#define DEVICE_TYPE servo_continuous
#elif defined(ON_PIN_DEVICE)
#define DEVICE_TYPE on_pin
#endif

#if defined(CAM_PHOTO_DEVICE)
#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
// define the number of bytes you want to access
#define EEPROM_SIZE 1
RTC_DATA_ATTR int bootCount = 0;
// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
int pictureNumber = 0;
// Take Picture with Camera
camera_fb_t * fb = NULL;
File file;
#endif


void setup() {
#ifndef CAM_PHOTO_DEVICE
  pinMode(LED_PIN,         OUTPUT);
  pinMode(BTN_PIN,      INPUT);
  ButtonConfig* buttonConfigBuiltIn = buttonBuiltIn.getButtonConfig();
  buttonConfigBuiltIn->setEventHandler(handleButtonEvent);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureClick);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfigBuiltIn->setLongPressDelay(LONG_PRESS);
  randomSeed(analogRead(0));
#else
  // randomSeed(analogRead(4));
#endif

  Serial.begin(115200); delay(500);

#if defined(SERVO_DEVICE)
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(USER_PIN, 150, 2500);
#elif defined(SERVO_CONTINUOUS_DEVICE)
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(USER_PIN, 1000, 2000);
#elif defined(ON_PIN_DEVICE)
  pinMode(USER_PIN, OUTPUT);
  Serial.println("on pin!");
#elif defined(CAM_PHOTO_DEVICE)
  setupCam();
#endif
  initPrefs();
  initWiFi();

  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  Serial.println(WiFi.macAddress());
  initESPNOW();
}
// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------

void loop() {
#ifndef CAM_PHOTO_DEVICE
  buttonBuiltIn.check();
#endif
  delay(10);
  // checkChannel();
}
