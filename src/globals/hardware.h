#pragma once

#include <Arduino.h>
#include "esp_camera.h"

#include "../hardware/push-button.cpp"
#include "../hardware/audio.cpp"

extern Button buttonUp;
extern Button buttonLeft;
extern Button buttonDown;
extern Button buttonRight;
extern Button buttonReturn;
extern Button buttonSelect;

extern camera_config_t cameraConfig;
