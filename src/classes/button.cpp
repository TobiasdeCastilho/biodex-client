#ifndef BUTTON

#include <Arduino.h>

class Button {
  private:
    int _pin;
		bool _last_state = false;
    bool _last_stable_state = false; 
    long _last_debounce_time = 0;  		
    long _debounce_delay = 50;     

  public:
    Button(int pin, std::string description) {
      _pin = pin;
      pinMode(_pin, INPUT_PULLDOWN); 
    }
    
    void read() {
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

#define BUTTON

#endif