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
// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------
bool isReceivedMsg = false;
#define MSGTIMEOUT 480000
int setWifi = 0;

uint8_t baseMac[6];
#define LED_PIN   2
#define USER_PIN 4
int BTN_PIN =   0;
//SET BASED OFF HUB
#define CHANNEL 1

//Acebutton
AceButton buttonBuiltIn(BTN_PIN);
void handleButtonEvent(AceButton*, uint8_t, uint8_t);
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


#define ON_PIN_DEVICE
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


void setup() {
  pinMode(LED_PIN,         OUTPUT);
  pinMode(BTN_PIN,      INPUT);

  ButtonConfig* buttonConfigBuiltIn = buttonBuiltIn.getButtonConfig();
  buttonConfigBuiltIn->setEventHandler(handleButtonEvent);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureClick);
  buttonConfigBuiltIn->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfigBuiltIn->setLongPressDelay(LONG_PRESS);
  Serial.begin(115200); delay(500);
  randomSeed(analogRead(0));

#if defined(SERVO_DEVICE)
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(USER_PIN, 1000, 2000);
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
  buttonBuiltIn.check();
  delay(10);
  checkChannel();
}
