#ifndef OPTIONS

#include <functional>
#include <vector>
#include "FS.h"
#include "LittleFS.h"
#include <TFT_eSPI.h>

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

class OptionList: public UIComponent {
	private:
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
		OptionList(Point position, Size optionsSize) : UIComponent(), _position(position), _optionsSize(optionsSize) {
			_listSize.width = _paddingHorizontal * 2 + ((_optionsSize.width + 5) * _options.size() - 5);
			_listSize.height = optionsSize.height + _paddingVertical * 2;
			_selectedOption = 0;
			_lastSelectedOption = -1;

			_visibilityChanged = true;
			_visible = true;
		}

		void consumeKeys() override {
			int lastOption = _selectedOption;

			switch(_listType) {
				case HORIZONTAL:
					if(btn_lf.consume()) {
						decreaseSelectedOption();
					}
					if(btn_rt.consume()) {
						increaseSelectedOption();
					}
					break;
				case VERTICAL:
					if(btn_up.consume()) {
						decreaseSelectedOption();
					}
					if(btn_dw.consume()) {
						increaseSelectedOption();
					}
					break;
			}

			_selectedOption = std::max(0, std::min(_selectedOption, (int)_options.size() - 1));
			if(_selectedOption != lastOption)
				_hasChanged = true;

			if(btn_sl.consume()) {
				activeSelectedOption();
			}
		}

		void addOptions(std::vector<Option> options) {
			for (const auto &option : options) {
				_options.push_back(option);
			}

			switch(_listType) {
				case HORIZONTAL:\
					_listSize.width = _paddingHorizontal * 2 + ((_optionsSize.width + 5) * _options.size() - 5);
					_listSize.height = _optionsSize.height + _paddingVertical * 2;
					break;
				case VERTICAL:
					_listSize.width = _optionsSize.width + _paddingHorizontal * 2;
					_listSize.height = _paddingVertical * 2 + ((_optionsSize.height + 5) * _options.size() - 5);
					break;
			}
			_selectedOption = 0;
			_lastSelectedOption = 0;

			_visibilityChanged = true;
		}

		void setInCenterX() {
			_position.x = (tft.width() - _listSize.width) / 2;

			_visibilityChanged = true;
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

			switch(_listType) {
				case HORIZONTAL:
					_listSize.width = _paddingHorizontal * 2 + ((_optionsSize.width + 5) * _options.size() - 5);
					_listSize.height = _optionsSize.height + _paddingVertical * 2;
					break;
				case VERTICAL:
					_listSize.width = _optionsSize.width + _paddingHorizontal * 2;
					_listSize.height = _paddingVertical * 2 + ((_optionsSize.height + 5) * _options.size() - 5);
					break;
			}
		}

		void render() override {
			if (!_hasChanged && !_visibilityChanged) return;
			_hasChanged = false;

			int currentX = _position.x + _paddingHorizontal;
			int currentY = _position.y + _paddingVertical;

			if (!_visibilityChanged) {
				Serial.printf("X: %d, Y: %d\n", currentX, currentY);
				Serial.printf("X: %d, Y: %d\n", currentX + (_listType == HORIZONTAL ? _selectedOption * (_optionsSize.width + 5) : 0), currentY + (_listType == VERTICAL ? _selectedOption * (_optionsSize.height + 5) : 0));
				Serial.printf("X: %d, Y: %d\n", currentX + (_listType == HORIZONTAL ? _lastSelectedOption * (_optionsSize.width + 5) : 0), currentY + (_listType == VERTICAL ? _lastSelectedOption * (_optionsSize.height + 5) : 0));

				tft.drawRoundRect(
					currentX + (_listType == HORIZONTAL ? _selectedOption * (_optionsSize.width + 5) : 0),
					currentY + (_listType == VERTICAL ? _selectedOption * (_optionsSize.height + 5) : 0),
					_optionsSize.height,
					_optionsSize.width,
					11,
					TFT_BLUE
				);
				tft.drawRoundRect(
					currentX + (_listType == HORIZONTAL ? _lastSelectedOption * (_optionsSize.width + 5) : 0),
					currentY + (_listType == VERTICAL ? _lastSelectedOption * (_optionsSize.height + 5) : 0),
					_optionsSize.height,
					_optionsSize.width,
					11,
					TFT_WHITE
				);

				return;
			}
			_visibilityChanged = false; // Reset visibility changed flag after verifying

			tft.setTextColor(TFT_WHITE);
			tft.setTextSize(2);

			tft.fillRoundRect(_position.x, _position.y, _listSize.width, _listSize.height, 11, TFT_DARKGREY);
			for (size_t i = 0; i < _options.size(); ++i) {
				if(_options[i].getFileBuffer() != nullptr)
					tft.pushImage(currentX + 3, currentY + 3, _optionsSize.width - 6, _optionsSize.height - 6, (uint16_t *) _options[i].getFileBuffer());
				else
					tft.drawString(_options[i].getLabel().c_str(), currentX + 3, currentY + 3, 2);
				tft.drawRoundRect(currentX, currentY, _optionsSize.width, _optionsSize.height, 11, i == _selectedOption ? TFT_BLUE : TFT_WHITE);

				switch(_listType) {
					case HORIZONTAL:
						currentX += _optionsSize.width + 5;
						break;
					case VERTICAL:
						currentY += _optionsSize.height + 5;
						break;
				}
			}
		}
};

#define OPTIONS

#endif
