#pragma once

#include <Arduino.h>

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

			_last_stable_state = reading;
			_last_state = reading;
    }

    bool consume() {
			bool state = _last_state;
			_last_state = false;

			return state;
    }
};
