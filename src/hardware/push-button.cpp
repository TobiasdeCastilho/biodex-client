#pragma once

#include <Arduino.h>

#include "audio.cpp"

class Button {
  private:
    int _pin;
		bool _last_state = false, _last_stable_state = false, _listen = true;
    long _last_debounce_time = 0, _debounce_delay = 50;

  public:
    Button(int pin, std::string description) {
      _pin = pin;
      pinMode(_pin, INPUT_PULLDOWN);
    }

		void stopListen() {
			_listen = false;
		}

		void listen() {
			_listen = true;
		}

    void read() {
			if(!_listen) return;

      bool reading = (digitalRead(_pin) == HIGH);
      if (reading == _last_stable_state){
				_last_state = false;
				return;
			}

      if(reading) {
        const uint8_t shutter_click[] = {
          128,128,128,128,128,
          192,240,255,240,192,144,120,144,
          192,240,255,240,192,144,120,108,
          120,132,140,135,130,128,128,128
        };
        play_raw_pwm(shutter_click, sizeof(shutter_click));
      }

			_last_stable_state = reading;
			_last_state = reading;
    }

    bool consume() {
			bool state = _last_state;
			_last_state = false;

			return state;
    }

    bool check() { return _last_state; };
};
