#pragma once

#include <string>
#include <functional>

#include "../globals/ui.h"

#include "component.cpp"
#include "themes.h"

class UIInput : public UIComponent {
	private:
		std::string text;
		size_t cursorPos = 0;

	public:
		UIInput(UIComponentSettings settings)
		: UIComponent(settings) {
			setFocusable(true);
			type = COMPONENT;
		}

		void setText(const std::string &value) {
			text = value;
			if(cursorPos > text.size()) cursorPos = text.size();
			markChanged();
		}

		std::string getText() const {
			return text;
		}

		void insertChar(char c) {
			text.insert(cursorPos, 1, c);
			cursorPos++;
			markChanged();
		}

		void backspace() {
			if(cursorPos == 0) return;
			text.erase(cursorPos-1, 1);
			cursorPos--;
			markChanged();
		}

		void render() override {
			if(!hasChanged && !focused) return;

			int bg = focused ? THEME_PRIMARY : settings.color;
			tft.fillRect(settings.position.x, settings.position.y,
						 settings.size.width, settings.size.height, bg);
			tft.drawRect(settings.position.x, settings.position.y,
						 settings.size.width, settings.size.height, THEME_BORDER);

			tft.setTextColor(THEME_TEXT, bg);
			tft.setTextDatum(ML_DATUM);
			tft.drawString(text.c_str(), settings.position.x + 4,
						   settings.position.y + settings.size.height/2);

			hasChanged = false;
		}
};
