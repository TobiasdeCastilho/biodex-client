#ifndef CAMERA

#include <Arduino.h>
#include "esp_camera.h"

#include "ui.cpp"

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

class Camera: public PrimitiveUI {		 
	private:
		camera_config_t _config;
	
	public:
		Camera(): PrimitiveUI() {			
			config();
			
      esp_err_t err;			
      while((err = esp_camera_init(&_config)) != ESP_OK) {       
        delay(1000);
      }			
		}		
		~Camera() {
			esp_camera_deinit();
		}

		void config() {
     	_config.ledc_channel = LEDC_CHANNEL_0;
      _config.ledc_timer = LEDC_TIMER_0;
      _config.pin_d0 = Y2_GPIO_NUM;
      _config.pin_d1 = Y3_GPIO_NUM;
      _config.pin_d2 = Y4_GPIO_NUM;
      _config.pin_d3 = Y5_GPIO_NUM;
      _config.pin_d4 = Y6_GPIO_NUM;
      _config.pin_d5 = Y7_GPIO_NUM;
      _config.pin_d6 = Y8_GPIO_NUM;
      _config.pin_d7 = Y9_GPIO_NUM;
      _config.pin_xclk = XCLK_GPIO_NUM;
      _config.pin_pclk = PCLK_GPIO_NUM;
      _config.pin_vsync = VSYNC_GPIO_NUM;
      _config.pin_href = HREF_GPIO_NUM;
      _config.pin_sccb_sda = SIOD_GPIO_NUM;
      _config.pin_sccb_scl = SIOC_GPIO_NUM;
      _config.pin_pwdn = PWDN_GPIO_NUM;
      _config.pin_reset = RESET_GPIO_NUM;
      _config.xclk_freq_hz = 20000000;

			_config.pixel_format = PIXFORMAT_RGB565;
			_config.jpeg_quality = 24;

			_config.frame_size = FRAMESIZE_CIF;
			_config.fb_count = 1;      
      _config.fb_location = CAMERA_FB_IN_DRAM;
      _config.grab_mode = CAMERA_GRAB_LATEST;

		}			

		void render() {
			camera_fb_t * fb = NULL;
      fb = esp_camera_fb_get();
			if(!fb) return;
			
			tft.pushImage(_definition.point.x, _definition.point.y, fb->width, fb->height, (uint16_t *)fb->buf);			

			esp_camera_fb_return(fb);
		}

		void consumeKeys() override {
			// Handle key inputs specific to the camera screen here
		}
};

#define CAMERA

#endif