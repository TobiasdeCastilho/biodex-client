#include <TFT_eSPI.h>

#include "button.cpp"
#include "ui.cpp"

#define KEYSIZE 48

class Keyboard : public PrimitiveUI {
	private:
		TFT_eSPI *_tft;				
		int _keyX = 0, _keyY = 0, _lastKeyX = 0, _lastKeyY = 0; // Current key position
		char _input[64] = {0}; // Buffer for input text
		int _inputLength = 0;
		Button *_btn_up, *_btn_dw, *_btn_lf, *_btn_rt, *_btn_rn, *_btn_sl;

		char keyMap[4][10] = {
			{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
			{'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
			{'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':'},
			{'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', ';'},			
		};

		void renderSpecialKeys(int key, bool active) {
			switch(key) {
				case 0: // Options change
					_tft->drawRect(0, 272, KEYSIZE, KEYSIZE, active ? TFT_BLUE : TFT_WHITE);
					if(_visibilityChanged) _tft->drawString("Opt", 10, 287);					
					break;
				case 1: // Case change
					_tft->drawRect(KEYSIZE, 272, KEYSIZE, KEYSIZE, active ? TFT_BLUE : TFT_WHITE);
					if(_visibilityChanged) _tft->drawString("Aa", 10 + KEYSIZE, 287);
					break;
				case 2: // Space
					_tft->drawRect(KEYSIZE * 2, 272, KEYSIZE * 4, KEYSIZE, active ? TFT_BLUE : TFT_WHITE);
					if(_visibilityChanged) _tft->drawString("Space", 10 + KEYSIZE * 2, 287);
					break;
				case 3: // Backspace
					_tft->drawRect(KEYSIZE * 6, 272, KEYSIZE * 2, KEYSIZE, active ? TFT_BLUE : TFT_WHITE);
					if(_visibilityChanged) _tft->drawString("Back", 10 + KEYSIZE * 6, 287);
					break;
				case 4: // Enter
					_tft->drawRect(KEYSIZE * 8, 272, KEYSIZE * 2, KEYSIZE, active ? TFT_BLUE : TFT_WHITE);
					if(_visibilityChanged) _tft->drawString("Enter", 10 + KEYSIZE * 8, 287);
					break;
			}
		}

	public:
		Keyboard(TFT_eSPI *tft, Button *btn_up, Button *btn_dw, Button *btn_lf, Button *btn_rt, Button *btn_rn, Button *btn_sl) : _tft(tft), _btn_up(btn_up), _btn_dw(btn_dw), _btn_lf(btn_lf), _btn_rt(btn_rt), _btn_rn(btn_rn), _btn_sl(btn_sl) {
			_visible = false;
			_hasChanged = false;
			_visibilityChanged = false;
		};
		~Keyboard() {}

	void render() {		
		if(!_hasChanged && !_visibilityChanged) return;				
		_hasChanged = false;		

		if(_visibilityChanged) _tft->fillScreen(TFT_BLACK);
		if(!_visible) {
			_visibilityChanged = false;
			return;
		}
										
		// Todo: Redraw only changed entries
		_tft->fillRect(0, 0, 480, 80, TFT_DARKGREY);
		_tft->setTextColor(TFT_WHITE);		
		_tft->drawString(_input, 10, 10);

		int line = 80;

		// redraw only changed keys
		if(!_visibilityChanged){
			if(_lastKeyY == 4) renderSpecialKeys(_lastKeyX, false); // Options change
			else _tft->drawRect(_lastKeyX * KEYSIZE, _lastKeyY * KEYSIZE + line, KEYSIZE, KEYSIZE, TFT_WHITE);
			_lastKeyX = _keyX;
			_lastKeyY = _keyY;

			if(_keyY == 4) renderSpecialKeys(_keyX, true); // Options change
			else if (_keyX < 10 && _keyY < 4) _tft->drawRect(_keyX * KEYSIZE, _keyY * KEYSIZE + line, KEYSIZE, KEYSIZE, TFT_BLUE);
			return;
		}
		_visibilityChanged = false; // Reset visibility changed flag after verifying
		
		int textX = 15;
		int textY = 15;
		_tft->setTextSize(2);

		for (int yIndex = 0; yIndex < 4; ++yIndex) {			
			int x = 0;
			for (int xIndex = 0; xIndex < 10; ++xIndex) {
				_tft->drawRect(x, line, KEYSIZE, KEYSIZE, _keyX == xIndex && _keyY == yIndex ? TFT_BLUE : TFT_WHITE);
				_tft->drawChar(keyMap[yIndex][xIndex], x + textX, line + textY);
				x += KEYSIZE;
			}			
			line += KEYSIZE;
		}
		
		// special keys
		renderSpecialKeys(0, _keyY == 4 && _keyX == 0); // Options change
		renderSpecialKeys(1, _keyY == 4 && _keyX == 1); // Case change
		renderSpecialKeys(2, _keyY == 4 && _keyX == 2); // Space
		renderSpecialKeys(3, _keyY == 4 && _keyX == 3); // Backspace
		renderSpecialKeys(4, _keyY == 4 && _keyX == 4); // Enter
	}

	void consumeKeys() {
		if(!_visible) return;		

		if (_btn_lf->consume()) { 			
			_keyX -= 1;
			_hasChanged = true;
		}
		if (_btn_rt->consume()) {						
			_keyX += 1;
			_hasChanged = true;
		}

		if (_btn_up->consume()) {			
			_keyY -= 1;
			_hasChanged = true;
		}
		if (_btn_dw->consume()) {			
			_keyY += 1;
			_hasChanged = true;
		}

		if (_keyY < 0) _keyY = 4;
		else if (_keyY > 4) _keyY = 0;

		if (_keyX < 0) _keyX = _keyY == 4 ? 4 : 9; // If in last row, wrap to 4
		else if (_keyY == 4 && _keyX > 4 || _keyX > 9) _keyX = 0;

		if (_btn_rn->consume()) {
			_visible = false;			
			_visibilityChanged = true;

			memset(_input, 0, sizeof(_input));
			_inputLength = 0;
		}			

		if (_btn_sl->consume()) {
			char key = '\0';
			// is special key
			if(_keyY == 4){
				switch(_keyX) {
					case 0: // Options change
						// ...
						break;
					case 1: // Case change
						// ...
						break;
					case 2: // Space
						Serial.println("CU");
						key = ' ';
						break;
					case 3: // Backspace
						if (_inputLength > 0) {
							_inputLength--;
							_input[_inputLength] = '\0';
							_hasChanged = true;
							return;
						}						
					case 4: // Enter
						
						break;					
				}
			} else {
				key = keyMap[_keyY][_keyX];
			}

			Serial.printf("Key pressed: %c, Is invalid: %d, Length: %d\n", key, key == '\0', _inputLength);

			if(key == '\0') return; // No key pressed
			if(_inputLength == 64) return;
			
			_hasChanged = true;
			_input[_inputLength] = key;
			_inputLength++;
		}		
	}	
};