#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"
#include <TJpg_Decoder.h>

#include "globals/ui.h"
#include "globals/manager.h"

#include "hardware/audio.cpp"

#include "screens/home.cpp"
#include "screens/wifi.cpp"
#include "screens/camera.cpp"

#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1

#define XCLK_GPIO_NUM    15
#define SIOD_GPIO_NUM    4
#define SIOC_GPIO_NUM    5

#define Y9_GPIO_NUM      16
#define Y8_GPIO_NUM      17
#define Y7_GPIO_NUM      18
#define Y6_GPIO_NUM      12
#define Y5_GPIO_NUM      10
#define Y4_GPIO_NUM      8
#define Y3_GPIO_NUM      9
#define Y2_GPIO_NUM      11

#define VSYNC_GPIO_NUM   6
#define HREF_GPIO_NUM    7
#define PCLK_GPIO_NUM    13

void renderTask(void *param){
	tft.fillScreen(THEME_BG);
	while(true){
		uiManager.render();
		vTaskDelay(34 / portTICK_PERIOD_MS);
	}
}

void buttonsRead(void *param) {
	while (true) {
		buttonUp.read();
		buttonLeft.read();
		buttonDown.read();
		buttonRight.read();
		buttonReturn.read();
		buttonSelect.read();

		vTaskDelay(34 / portTICK_PERIOD_MS);
	}
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0; // Evita overflow
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void setup() {
	Serial.begin(115200);

	// if (!psramInit()) {
 //    Serial.println("Falha ao inicializar PSRAM!");
 //  } else {
 //    Serial.println("PSRAM inicializada com sucesso!");
 //  }

  pwm_audio_init();

  TJpgDec.setJpgScale(1); // Sem escala
  TJpgDec.setCallback(tft_output);

	if(!LittleFS.begin() && (!LittleFS.exists("/data") || !LittleFS.mkdir("/data"))) return;
	data.load();

	// Pin configuration (your pins are correct)
  cameraConfig.ledc_channel = LEDC_CHANNEL_0;
  cameraConfig.ledc_timer = LEDC_TIMER_0;
  cameraConfig.pin_d0 = Y2_GPIO_NUM;
  cameraConfig.pin_d1 = Y3_GPIO_NUM;
  cameraConfig.pin_d2 = Y4_GPIO_NUM;
  cameraConfig.pin_d3 = Y5_GPIO_NUM;
  cameraConfig.pin_d4 = Y6_GPIO_NUM;
  cameraConfig.pin_d5 = Y7_GPIO_NUM;
  cameraConfig.pin_d6 = Y8_GPIO_NUM;
  cameraConfig.pin_d7 = Y9_GPIO_NUM;
  cameraConfig.pin_xclk = XCLK_GPIO_NUM;
  cameraConfig.pin_pclk = PCLK_GPIO_NUM;
  cameraConfig.pin_vsync = VSYNC_GPIO_NUM;
  cameraConfig.pin_href = HREF_GPIO_NUM;
  cameraConfig.pin_sccb_sda = SIOD_GPIO_NUM;
  cameraConfig.pin_sccb_scl = SIOC_GPIO_NUM;
  cameraConfig.pin_pwdn = PWDN_GPIO_NUM;
  cameraConfig.pin_reset = RESET_GPIO_NUM;
  cameraConfig.xclk_freq_hz = 20000000; // 20MHz is good

  // PSRAM-optimized configuration
  // if(psramFound()) {
  //   Serial.println("PSRAM detectado - configurando para RGB565");

  //   // Usar resolução que melhor se adapta à sua tela 480x360
  //   cameraConfig.pixel_format = PIXFORMAT_RGB565; // Ideal para TFT_eSPI
  //   cameraConfig.frame_size = FRAMESIZE_HVGA;     // 480x320 - QUASE PERFEITO para 480x360
  //   // Ou use FRAMESIZE_NHD (640x360) e faça scaling para 480x360

  //   cameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
  //   cameraConfig.fb_count = 1; // 1 frame buffer é suficiente para display
  //   cameraConfig.grab_mode = CAMERA_GRAB_LATEST;

  // } else {
    // Serial.println("Sem PSRAM - usando JPEG");
    cameraConfig.pixel_format = PIXFORMAT_JPEG;
    cameraConfig.jpeg_quality = 8;
    cameraConfig.frame_size = FRAMESIZE_HQVGA;
    cameraConfig.fb_location = CAMERA_FB_IN_DRAM;
    cameraConfig.fb_count = 1;
    cameraConfig.grab_mode = CAMERA_GRAB_LATEST;
  // }

  // esp_err_t err;
  // while ((err = esp_camera_init(&cameraConfig)) != ESP_OK) delay(1000);

  std::string ssid = data.getWiFiId();
  std::string password = data.getWiFiPassword();
  if(ssid.size() && password.size()){
    int maxTries = 20;

    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED && maxTries-- > 0) delay(500);
    if(WiFi.status() != WL_CONNECTED) {
      data.setWiFiId("");
      data.setWifiPassword("");
    }
  }

	tft.init();
	tft.setRotation(3);

	tft.invertDisplay(true);
	tft.setSwapBytes(true);

	tft.fillScreen(THEME_BG);

	uiManager.addScreen(createHomeScreen);
	uiManager.addScreen(createCameraScreen);
	uiManager.addScreen(createWiFiSettingsScreen);
	uiManager.setCurrentScreen(0);

	xTaskCreatePinnedToCore(
		renderTask,
		"Render",
		4096,
		NULL,
		1,
		NULL,
		1
	);

	xTaskCreatePinnedToCore(
	  buttonsRead,
	  "ButtonsRead",
	  2048,
	  NULL,
	  2,
	  NULL,
	  0
  );
}

void loop() {
	uiManager.consumeKeys();
	delay(20);
}
