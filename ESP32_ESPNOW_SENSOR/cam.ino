#if defined(CAM_MOVEMENT_DEVICE)
bool setup_camera(framesize_t frameSize) {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = frameSize;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  bool ok = esp_camera_init(&config) == ESP_OK;

  sensor_t *sensor = esp_camera_sensor_get();
  sensor->set_framesize(sensor, frameSize);

  return ok;
}

/**
   Capture image and do down-sampling
*/
bool capture_still() {
  camera_fb_t *frame_buffer = esp_camera_fb_get();

  if (!frame_buffer)
    return false;

  // set all 0s in current frame
  for (int y = 0; y < H; y++)
    for (int x = 0; x < W; x++)
      current_frame[y][x] = 0;


  // down-sample image in blocks
  for (uint32_t i = 0; i < WIDTH * HEIGHT; i++) {
    const uint16_t x = i % WIDTH;
    const uint16_t y = floor(i / WIDTH);
    const uint8_t block_x = floor(x / BLOCK_SIZE);
    const uint8_t block_y = floor(y / BLOCK_SIZE);
    const uint8_t pixel = frame_buffer->buf[i];
    const uint16_t current = current_frame[block_y][block_x];

    // average pixels in block (accumulate)
    current_frame[block_y][block_x] += pixel;
  }

  // average pixels in block (rescale)
  for (int y = 0; y < H; y++)
    for (int x = 0; x < W; x++)
      current_frame[y][x] /= BLOCK_SIZE * BLOCK_SIZE;



  esp_camera_fb_return(frame_buffer);

  return true;
}


/**
   Compute the number of different blocks
   If there are enough, then motion happened
*/
bool motion_detect() {
  uint16_t changes = 0;
  const uint16_t blocks = (WIDTH * HEIGHT) / (BLOCK_SIZE * BLOCK_SIZE);

  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      float current = current_frame[y][x];
      float prev = prev_frame[y][x];
      float delta = abs(current - prev) / prev;

      if (delta >= BLOCK_DIFF_THRESHOLD) {


        changes += 1;
      }
    }
  }

  Serial.print("Changed ");
  Serial.print(changes);
  Serial.print(" out of ");
  Serial.println(blocks);

  return (1.0 * changes / blocks) > IMAGE_DIFF_THRESHOLD;
}


/**
   Copy current frame to previous
*/
void update_frame() {
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      prev_frame[y][x] = current_frame[y][x];
    }
  }
}

/**
   For serial debugging
   @param frame
*/
void print_frame(uint16_t frame[H][W]) {
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      Serial.print(frame[y][x]);
      Serial.print('\t');
    }

    Serial.println();
  }
}

int detects = 0;

void checkCam() {
  if (!capture_still()) {
    Serial.println("Failed capture");
    delay(3000);

    return;
  }

  if (motion_detect()) {
    detects++;
    if (detects == 2) {
      Serial.println("Motion detected");
      detects = 0;
      sendSensor();
    }
  }

  update_frame();
  Serial.println("=================");
}
#endif
