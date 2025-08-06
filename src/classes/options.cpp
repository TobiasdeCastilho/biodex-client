#ifndef OPTIONS

#include <functional>
#include <vector>
#include "FS.h"
#include "LittleFS.h"
#include <TFT_eSPI.h>

#include "../types/screen.cpp"
#include "ui.cpp"

typedef std::function<void(bool active)> OptionCallback;
class Option {
	private: 	
		bool _active = false;				
		uint8_t *_buffer;
		std::string _label;
		OptionCallback _callback;
	public:
		Option(OptionCallback callback, std::string iconPath, std::string label) : _callback(callback), _label(label) {			
			_buffer = nullptr;

			if(!iconPath.empty()) {
				File iconFile = LittleFS.open(("/icons/" + iconPath).c_str(), "r");
				
				_buffer = new uint8_t[iconFile.size()];
	
				Serial.print("Loading icon: ");
				Serial.println(iconFile.size());
	
				iconFile.readBytes((char *)_buffer, iconFile.size());
				iconFile.close();
			}			
		}		

		void select() {
			_active = !_active;
			_callback(_active);
		}		

		uint8_t * getFileBuffer() {
			return _buffer;
		}

		std::string getLabel() {
			return _label;
		}			
};

typedef enum {
	HORIZONTAL,
	VERTICAL
} OptionListType;

class OptionList: public PrimitiveUI {
	private:
		TFT_eSPI *_tft;
		std::vector<Option> _options;		
		Point _position;
		Size _listSize;
		Size _optionsSize;		
		OptionListType _listType = HORIZONTAL;
		bool _focused = false;		
		int _selectedOption, _lastSelectedOption = -1;

		int _paddingHorizontal = 5;
		int _paddingVertical = 5;

	public:
		OptionList(TFT_eSPI *tft, Point position, Size optionsSize) : _position(position), _optionsSize(optionsSize), _tft(tft) {
			_listSize.width = _paddingHorizontal * 2 + ((_optionsSize.width + 5) * _options.size() - 5);
			_listSize.height = optionsSize.height + _paddingVertical * 2;
			_selectedOption = 0;
			_lastSelectedOption = -1;
			
			_visibilityChanged = true;
			_visible = true;
		}
		
		void addOptions(std::vector<Option> options) {
			_options = options;
			_listSize.width = _paddingHorizontal * 2 + ((_optionsSize.width + 5) * _options.size() - 5);
			_listSize.height = _optionsSize.height + _paddingVertical * 2;
			_selectedOption = 0;
			_lastSelectedOption = -1;
			
			_visibilityChanged = true;
		}
		
		void setInCenterX() {
			_position.x = (_tft->width() - _listSize.width) / 2;
			
			_visibilityChanged = true;			
		}

		void render() {
			if (!_hasChanged && !_visibilityChanged) return;
			_hasChanged = false;						

			int currentX = _position.x + _paddingHorizontal;
			int yPosition = _position.y + _paddingVertical;
			if (!_visibilityChanged) {
				_tft->drawRoundRect(
					currentX + _lastSelectedOption * (_optionsSize.height + 5),
					yPosition,
					_optionsSize.height,
					_optionsSize.width,
					11,
					TFT_WHITE
				);
				_tft->drawRoundRect(
					currentX + _selectedOption * (_optionsSize.height + 5),
					yPosition,
					_optionsSize.height,
					_optionsSize.width,
					11,
					TFT_BLUE
				);

				return;
			}
			_visibilityChanged = false; // Reset visibility changed flag after verifying

			_tft->setTextColor(TFT_WHITE);

			_tft->fillRoundRect(_position.x, _position.y, _listSize.width, _listSize.height, 11, TFT_DARKGREY);
			
			switch(_listType) {
				case HORIZONTAL:
					for (size_t i = 0; i < _options.size(); ++i) {
						if(_options[i].getFileBuffer() != nullptr) {
							_tft->pushImage(currentX + 3, yPosition + 3, _optionsSize.width - 6, _optionsSize.height - 6, _options[i].getFileBuffer());
						} else {
							_tft->drawString(_options[i].getLabel().c_str(), currentX + 3, yPosition + 3, 2);					
						}
						_tft->drawRoundRect(currentX, yPosition, _optionsSize.width, _optionsSize.height, 11, i == _selectedOption ? TFT_BLUE : TFT_WHITE);
						currentX += _optionsSize.width + 5;
					}					
					break;
				case VERTICAL:
					for (size_t i = 0; i < _options.size(); ++i) {
						if(_options[i].getFileBuffer() != nullptr) {
							_tft->pushImage(currentX + 3, yPosition + 3, _optionsSize.width - 6, _optionsSize.height - 6, _options[i].getFileBuffer());
						} else {
							_tft->drawString(_options[i].getLabel().c_str(), currentX + 3, yPosition + 3, 2);					
						}
						_tft->drawRoundRect(currentX, yPosition, _optionsSize.width, _optionsSize.height, 11, i == _selectedOption ? TFT_BLUE : TFT_WHITE);
						yPosition += _optionsSize.height + 5;
					}
					break;
			}						
		}
		
		void activeSelectedOption() {
			if (_selectedOption < 0 || _selectedOption >= _options.size()) return;			
				_options[_selectedOption].select();			
		}

		void increaseSelectedOption() {			
			_lastSelectedOption = _selectedOption;
			_hasChanged = true;

			if (_selectedOption == _options.size() - 1) {
				_selectedOption = 0;
				return;
			}		

			_selectedOption++;						
		}

		void decreaseSelectedOption() {
			_lastSelectedOption = _selectedOption;
			_hasChanged = true;

			if (_selectedOption == 0) {
				_selectedOption = _options.size() - 1;
				return;
			}
			
			_selectedOption--;			
		}

		void setListType(OptionListType type) {
			_listType = type;
			_hasChanged = true;
			_visibilityChanged = true;
		}
};

#define OPTIONS

#endif