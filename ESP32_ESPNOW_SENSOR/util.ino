void blinkLed() {
  digitalWrite(LED, 1);
  delay(40);
  digitalWrite(LED, 0);
}

bool isLong = false;
// button functions
void handleButtonEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (button->getPin()) {
      Serial.println("got event");
    case 0:
      switch (eventType) {
        case AceButton::kEventPressed:
          break;
        case AceButton::kEventReleased:
          if (isLong == false) {
            //do something on long press
            setSavedChannel(1);
            ESP.restart();
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


void initPrefs() {
  prefs.begin("channelSettings");
}

void readSensor() {
#if(DEVICE_TYPE == cap_touch)
  int touch = touchRead(T0);
  if (touch < 50 && isPressed == false) {
    isPressed = true;
    sendSensor();
  }
  if (touch > 50 && isPressed == true) {
    isPressed = false;
  }
#elif(DEVICE_TYPE == simple_switch)
  int pressed = digitalRead(SENSOR_PIN);
  if (pressed == false && isPressed == false) {
    isPressed = true;
    sendSensor();
  }
  if (pressed == true && isPressed == true) {
    isPressed = false;
  }
  delay(30);
#elif(DEVICE_TYPE == cam_movement)
  checkCam();
#elif (DEVICE_TYPE == radar)
  int pressed = digitalRead(SENSOR_PIN);
  if (pressed == true && isPressed == false) {
    isPressed = true;
    sendSensor();
  }
  if (pressed == false && isPressed == true) {
    isPressed = false;
  }
  delay(30);
#endif
}
