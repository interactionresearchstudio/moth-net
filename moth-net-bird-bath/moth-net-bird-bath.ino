#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#define CHANNEL 1
#define MY_ID 1
#define MSG_SIZE 12

#define LED 2
#define LED_ON 1
#define LED_OFF 0

enum sensorTypes {
  cap_touch,
  simple_switch,
  cam_movement,
  radar,
  cam_photo
};


// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

sensorTypes sensorType;
byte ID;
int eventVal;

// Define variables to store incoming readings
sensorTypes incomingSensorType;
byte incomingID;
int incomingEventVal;

//Structure example to send data
typedef struct struct_message {
  byte ID;
  sensorTypes sensors;
  int eventVal;
} struct_message;

unsigned long prevMillisSensorPoll;
#define SENSOR_POLL 160
#define TOUCHPIN 4
#define TOUCH_THRESH 15
int currTouch;
bool isTouched = false;

// Variable to store if sending data was successful
String success;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;
struct_message outgoingReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  }
  else {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (len == MSG_SIZE) {
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    Serial.print("Bytes received: ");
    Serial.println(len);
    incomingSensorType = incomingReadings.sensors;
    incomingID = incomingReadings.ID;
    incomingEventVal = incomingReadings.eventVal;
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  //esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {


  if (millis() - prevMillisSensorPoll > SENSOR_POLL) {
    prevMillisSensorPoll = millis();
    currTouch = touchRead(TOUCHPIN);
    Serial.println(currTouch);
    if (currTouch < TOUCH_THRESH && isTouched == false) {
      delay(5);
      currTouch = touchRead(TOUCHPIN);
      if (currTouch < TOUCH_THRESH) {
        isTouched = true;
        digitalWrite(LED, LED_ON);
        Serial.println("touch triggered");

        //set sensor struct
        outgoingReadings.sensors = cap_touch;
        outgoingReadings.ID = MY_ID;
        outgoingReadings.eventVal = 1;

        sendESPNOW();
      }
    } else if (currTouch > TOUCH_THRESH && isTouched == true) {
      isTouched = false;
    } else if (currTouch > TOUCH_THRESH && isTouched == false) {
      digitalWrite(LED, LED_OFF);
    }
  }

}


void sendESPNOW() {
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(1000);
}
