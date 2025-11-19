#pragma once

#include "driver/ledc.h"
#include "esp_log.h"
#include <string.h>
#include <unistd.h>

#define PWM_GPIO     14        // pino que vai ao IN+ do PAM8403
#define PWM_FREQ     22050
#define PWM_TIMER    LEDC_TIMER_0
#define PWM_MODE     LEDC_LOW_SPEED_MODE   // <<<<< único modo do S3
#define PWM_CHANNEL  LEDC_CHANNEL_0
#define PWM_RES      LEDC_TIMER_8_BIT      // 0-255

static const char *TAG = "PWM-AUDIO";

inline void pwm_audio_init() {
  ledc_timer_config_t timer_cfg;
  timer_cfg.speed_mode = PWM_MODE;
  timer_cfg.timer_num = PWM_TIMER;
  timer_cfg.duty_resolution = PWM_RES;
  timer_cfg.freq_hz = PWM_FREQ;
  timer_cfg.clk_cfg = LEDC_AUTO_CLK;
  ledc_timer_config(&timer_cfg);

  ledc_channel_config_t ch_cfg;
  ch_cfg.gpio_num = PWM_GPIO;
  ch_cfg.speed_mode = PWM_MODE;
  ch_cfg.channel = PWM_CHANNEL;
  ch_cfg.timer_sel = PWM_TIMER;
  ch_cfg.duty = 128;   // centro (silêncio)
  ch_cfg.hpoint = 0;
  ledc_channel_config(&ch_cfg);

  ESP_LOGI(TAG, "PWM audio init: GPIO %d, %d Hz", PWM_GPIO, PWM_FREQ);
}

inline void play_raw_pwm(const uint8_t *data, size_t len) {
  if (!data || !len) return;

  const uint32_t delay_us = 1000000UL / PWM_FREQ;   // ~45 µs @ 22 kHz

  for (size_t i = 0; i < len; ++i) {
      ledc_set_duty(PWM_MODE, PWM_CHANNEL, data[i]);
      ledc_update_duty(PWM_MODE, PWM_CHANNEL);
      ets_delay_us(delay_us);
  }

  /* volta ao silêncio */
  ledc_set_duty(PWM_MODE, PWM_CHANNEL, 128);
  ledc_update_duty(PWM_MODE, PWM_CHANNEL);
}
