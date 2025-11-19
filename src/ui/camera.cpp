#pragma once

#include <Arduino.h>
#include "esp_camera.h"
#include <TJpg_Decoder.h>

#include "../globals/hardware.h"
#include "../globals/ui.h"

#include "component.cpp"

typedef std::function<void(camera_fb_t*)> CameraCallback;

class UICamera: public UIComponent {
  private:
    camera_fb_t *fb = nullptr;
    bool photo = false;
    bool forcedRender = false;
    CameraCallback callback = nullptr;

  public:

    UICamera() : UIComponent({{SCREEN_WIDTH, SCREEN_HEIGHT}, {0, 0}, THEME_BG}) {
      setFocusable(true);
      markChanged();
    }

    ~UICamera() {}

    void render() override {
      if (photo && !forcedRender) return;
      if(!forcedRender) {
        fb = esp_camera_fb_get();
        if (!fb) return;
      }
      forcedRender = false;
      TJpgDec.drawJpg(settings.position.x, settings.position.y, fb->buf, fb->len);

      esp_camera_fb_return(fb);
    }

    void forceRender() {
      forcedRender = true;
    }

    bool consumeKeys() override {
      if (buttonSelect.consume()) {
        photo = true;

        const uint8_t shutter_click[] = {
          128,128,128,128,128,
          192,240,255,240,192,144,120,144,
          192,240,255,240,192,144,120,108,
          120,132,140,135,130,128,128,128
        };
        play_raw_pwm(shutter_click, sizeof(shutter_click));

        if(callback) callback(fb);
      }

      return false;
    }

    void resetPhoto() {
      photo = false;
    }

    void setCallback(CameraCallback cb) {
      callback = cb;
    }
};
