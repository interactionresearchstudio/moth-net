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

void blinkLed() {
  digitalWrite(LED_PIN, 1);
  delay(40);
  digitalWrite(LED_PIN, 0);
}

void initPrefs() {
  prefs.begin("channelSettings");
}

void performAction(int valueIn) {
  Serial.print("Value is: ");
  Serial.println(valueIn);
#if defined(SERVO_DEVICE)
  myservo.write(180);    // tell servo to go to position in variable 'pos'
  delay(1000);
  myservo.write(0);    // tell servo to go to position in variable 'pos'
  delay(1000);
#elif defined(CAM_PHOTO_DEVICE)
  takePhoto();
#elif defined(SERVO_CONTINUOUS_DEVICE)
  myservo.write(180);
  delay(2000);
  myservo.write(0);
  delay(2000);
  myservo.write(90);
#elif defined(ON_PIN_DEVICE)
  Serial.println("ON");
  digitalWrite(USER_PIN, 1);
  delay(3000);
  digitalWrite(USER_PIN, 0);
#elif degfined(RGB_LED_DEVICE)
  leds[0] = CHSV(valueIn, 255, 255);
  FastLED.show();
  delay(1000);
  leds[0] = CHSV(valueIn, 255, 0);
  FastLED.show();
#endif
}
