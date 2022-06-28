void blinkLed() {
  digitalWrite(LED, 1);
  delay(50);
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
            sendSensor();
          } else {
            isLong = false;
          }
          break;
        case AceButton::kEventLongPressed:
          isLong = true;
          setSavedChannel(1);
          ESP.restart();
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

void checkRadar() {
  int pressed = digitalRead(SENSOR_PIN);
  if (pressed == true && isPressed == false) {
    isPressed = true;
    sendSensor();
  }
  if (pressed == false && isPressed == true) {
    isPressed = false;
  }
  delay(30);
}

void checkSwitch() {
  int pressed = digitalRead(SENSOR_PIN);
  if (pressed == false && isPressed == false) {
    isPressed = true;
    sendSensor();
  }
  if (pressed == true && isPressed == true) {
    isPressed = false;
  }
  delay(30);
}

void checkCap() {
  int touch = touchRead(T0);
  if (touch < 50 && isPressed == false) {
    isPressed = true;
    sendSensor();
  }
  if (touch > 50 && isPressed == true) {
    isPressed = false;
  }
}

void checkHall() {
  int hall = hallRead();
  if (hall > 48 && isPressed == false) {
    isPressed = true;
    sendSensor();
    delay(50);
  }
  if (hall > 50 && isPressed == true) {
    isPressed = false;
  }
  delay(5);
}
