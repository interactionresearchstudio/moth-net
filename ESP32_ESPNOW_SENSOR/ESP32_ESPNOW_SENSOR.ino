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

bool isReceivedMsg = false;
#define MSGTIMEOUT 480000
// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------

int setWifi = 0;

uint8_t baseMac[6];
#define LED_PIN   2
int BTN_PIN =   0;
//SET BASED OFF HUB
#define CHANNEL 1

//Acebutton
AceButton buttonBuiltIn(BTN_PIN);

void handleButtonEvent(AceButton*, uint8_t, uint8_t);
// ----------------------------------------------------------------------------
// Definition of global constants
// ----------------------------------------------------------------------------

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


// Button debouncing
const uint8_t DEBOUNCE_DELAY = 10; // in milliseconds


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

// ----------------------------------------------------------------------------
// Definition of the LED component
// ----------------------------------------------------------------------------
struct Led {
  // state variables
  uint8_t pin;
  bool    on;

  // methods
  void update() {
    digitalWrite(pin, on ? HIGH : LOW);
  }
};

// ----------------------------------------------------------------------------
// Definition of global variables
// ----------------------------------------------------------------------------
Led    led         = { LED_PIN, false };

// ----------------------------------------------------------------------------
// SPIFFS initialization
// ----------------------------------------------------------------------------

void initPrefs() {
  prefs.begin("channelSettings");
}

// ----------------------------------------------------------------------------
// Connecting to the ESPNOW
// ----------------------------------------------------------------------------

void setEspNowChannel(int ch) {
  Serial.print("Channel: ");
  Serial.println(ch);
  setWifiChannel(ch);

  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

  if (esp_now_is_peer_exist(broadcastAddress)) {
    esp_now_del_peer(broadcastAddress);
  }

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = ch;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }


}

void initESPNOW() {


  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    //ESP.restart();
  }

  int ch = checkSavedChannel();
  setEspNowChannel(ch);

  esp_now_register_recv_cb(onDataReceive);
  esp_now_register_send_cb(OnDataSent);

}

int checkSavedChannel() {
  int chan = prefs.getInt("channel", 0);
  if (chan == 0) {
    Serial.println("no channel saved");
    return CHANNEL;
  } else {
    Serial.print("connecting to channel ");
    Serial.println(chan);
    return chan;
  }
}

void setSavedChannel(int chan) {
  prefs.putInt("channel", chan);
  Serial.println("saving channel to preferences");
}



// ----------------------------------------------------------------------------
// Connecting to the WiFi network
// ----------------------------------------------------------------------------

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}


// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void setup() {
  pinMode(led.pin,         OUTPUT);
  pinMode(BTN_PIN,      INPUT);

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

// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------

bool isPressed = false;

void loop() {
  buttonBuiltIn.check();
  int touch = touchRead(T0);
  if (touch < 50 && isPressed == false) {
    isPressed = true;
    sendSensor();
  }
  if (touch > 50 && isPressed == true) {
    isPressed = false;
  }
  delay(30);
  // led.update();
  checkChannel();
}

void checkChannel() {
  if (isReceivedMsg == false) {
    if (millis() > MSGTIMEOUT) {
      Serial.println("Never received heartbeat, setting back to channel 1");
      setSavedChannel(1);
      ESP.restart();
    }
  }
}

boolean array_cmp(uint8_t *a, uint8_t *b, int len_a, int len_b) {
  int n;

  // if their lengths are different, return false
  if (len_a != len_b) return false;

  // test each element to be the same. if not, return false
  for (n = 0; n < len_a; n++) if (a[n] != b[n]) return false;

  //ok, if we have not returned yet, they are equal :)
  return true;
}


// On data received
void onDataReceive(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  //Add CRC
  if (len == sizeof(msg)) {
    memcpy(&msg, incomingData, sizeof(msg));
    char macStr[18];
    snprintf(macStr, sizeof(macStr), " % 02x: % 02x: % 02x: % 02x: % 02x: % 02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print("Received from: ");
    Serial.println(macStr);
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Sensor type: ");
    Serial.println(msg.sensors);
    if (msg.function == wifiChannel) {
      if (msg.eventVal != getWifiChannel()) {
        setSavedChannel(msg.eventVal);
        setEspNowChannel(msg.eventVal);
        delay(5000);
        sendHeartbeat();
      }
    } else if (msg.function == blinking) {
      Serial.println("Blink!");
      blinkLed();
    } else if (msg.function == heartbeatREQ) {
      isReceivedMsg = true;
      //delay so they don't all send at the same time
      delay(random(100));
      sendHeartbeat();
    } else if (array_cmp(msg.mac, baseMac, 6, 6)) {
    }
    //  if (msg.function == action) {
    //   Serial.println("servo!");
    //    blinkLed();
    //    delay(20);
    //    blinkLed();
    //}
  }
}

void blinkLed() {
  digitalWrite(LED_PIN, 1);
  delay(40);
  digitalWrite(LED_PIN, 0);
}

void sendESPNOW() {
  // Send message via ESP-NOW
  outgoingReadings.function = sensor;
  outgoingReadings.sensors = cap_touch;
  outgoingReadings.eventVal = 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

void sendSensor() {
  // Send message via ESP-NOW

  outgoingReadings.function = sensor;
  outgoingReadings.sensors = cap_touch;
  outgoingReadings.eventVal = 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}


void sendHeartbeat() {
  // Send message via ESP-NOW
  outgoingReadings.function = heartbeat;
  outgoingReadings.sensors = cap_touch;
  outgoingReadings.eventVal = 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

String success;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status: \t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success : )";
  }
  else {
    success = "Delivery Fail : (";
  }
}

bool isLong = false;
// button functions
void handleButtonEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (button->getPin()) {
    case 0:
      switch (eventType) {
        case AceButton::kEventPressed:
          break;
        case AceButton::kEventReleased:
          if (isLong == false) {
            sendESPNOW();
          } else {
            isLong = false;
          }
          break;
        case AceButton::kEventLongPressed:
          isLong = true;
          sendSensor();
          break;
        case AceButton::kEventRepeatPressed:
          break;
      }
      break;
  }
}

int getWifiChannel() {
  return setWifi;
}

void setWifiChannel(int wifi) {
  setWifi = wifi;
}
