#include <esp_now.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Preferences.h>

#define LED 2

Preferences preferences;

WiFiManager wm;

uint8_t globalMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

enum sensorTypes {
  cap_touch,
  simple_switch,
  cam_movement,
  radar,
  cam_photo
};

//Structure example to send data
typedef struct struct_message {
  byte ID;
  sensorTypes sensors;
  int eventVal;
} struct_message;

struct_message msg;

String aio_key, aio_user;

WiFiClient espClient;
PubSubClient client(espClient);

boolean readyToSend = false;
String nextFeed, nextValue;

void setup() {

  // start the serial connection
  Serial.begin(115200);
  pinMode(0, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  digitalWrite(LED, LOW);

  // Setup ESP Now
  WiFi.mode(WIFI_AP_STA);

  // WiFi manager
  WiFiManagerParameter aio_user_value("aio_user", "Adafruit IO username", "", 40);
  WiFiManagerParameter aio_key_value("aio_key", "Adafruit IO key", "", 40);
  wm.addParameter(&aio_key_value);
  wm.addParameter(&aio_user_value);

  if (digitalRead(0) == LOW) {
    wm.startConfigPortal("moth-net", "badgersandfoxes");
  }
  else {
    wm.autoConnect("moth-net", "badgersandfoxes");
  }

  int ch = WiFi.channel();
  Serial.print("Channel: ");
  Serial.println(ch);
  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

  if (strlen(aio_user_value.getValue()) > 0) {
    Serial.println("Saving custom fields");
    preferences.begin("moth-net", false);
    preferences.putString("aio-user", aio_user_value.getValue());
    preferences.end();
  }
  if (strlen(aio_key_value.getValue()) > 0) {
    Serial.println("Saving custom fields");
    preferences.begin("moth-net", false);
    preferences.putString("aio-key", aio_key_value.getValue());
    preferences.end();
  }

  Serial.println("Loading preferences...");
  preferences.begin("moth-net", false);
  aio_key = preferences.getString("aio-key", "");
  aio_user = preferences.getString("aio-user", "");
  preferences.end();

  Serial.print("AIO key: ");
  Serial.println(aio_key);
  Serial.print("AIO user: ");
  Serial.println(aio_user);

  connectToMqtt();

  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    //ESP.restart();
  }
  esp_now_register_recv_cb(onDataReceive);
}

void loop() {
  client.loop();
}

void connectToMqtt() {
  // Connect to WiFi
  //wm.autoConnect("moth-net", "badgersandfoxes");
  //Serial.println("Connected to WiFi.");
  // Connect to MQTT
  client.setServer("io.adafruit.com", 1883);
  client.setCallback(mqttCallback);
  if (client.connect("moth-net-client", aio_user.c_str(), aio_key.c_str())) {
    Serial.println("Connected to MQTT server");
    String feedName = aio_user + String("/f/moth-hub-led");
    client.subscribe(feedName.c_str());
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED, HIGH);
      delay(50);
      digitalWrite(LED, LOW);
      delay(50);
    }
  }
  else {
    Serial.println("Failed to connect to MQTT server!");
    digitalWrite(LED, HIGH);
  }
}

void disconnectFromMqtt() {
  WiFi.mode(WIFI_STA);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1') {
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
  }
  Serial.println();
}

// On data received
void onDataReceive(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&msg, incomingData, sizeof(msg));
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.print("Received from: ");
  Serial.println(macStr);
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Sensor type: ");
  Serial.println(msg.sensors);
  Serial.print("ID: ");
  Serial.println(msg.ID);
  Serial.print("Value: ");
  Serial.println(msg.eventVal);

  String aio_message = "{\"value\": " + String(msg.eventVal) + ",\"lat\": 51.516651, \"lon\": -0.076840}";

  if (msg.sensors == cap_touch) {
    client.publish((aio_user + String("/f/moth-net.bird-bath")).c_str(), aio_message.c_str());
  } 
  else if (msg.sensors == cam_movement) {
    client.publish((aio_user + String("/f/moth-net.camera")).c_str(), aio_message.c_str());
  }
  else if (msg.sensors == simple_switch) {
    client.publish((aio_user + String("/f/moth-net.switch")).c_str(), aio_message.c_str());
  }
}
