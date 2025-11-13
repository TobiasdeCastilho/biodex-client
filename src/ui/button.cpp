#pragma once

#include <functional>
#include <string>

#include "component.cpp"
#include "themes.h"

typedef std::function<void()> ButtonCallback;

class UIButton : public UIComponent {
	private:
		std::string label;
		ButtonCallback callback;

	public:
		UIButton(UIComponentSettings settings, const std::string &text, ButtonCallback cb)
		: UIComponent(settings), label(text), callback(cb) {
			setFocusable(true);
			type = BUTTON;
		}

		void render() override {
			if(!hasChanged) return;

			int fillColor = focused ? THEME_PRIMARY : settings.color;
			int borderColor = focused ? THEME_BORDER : THEME_ACCENT;

			tft.fillRect(
				settings.position.x,
				settings.position.y,
				settings.size.width,
				settings.size.height,
				fillColor
			);

			tft.drawRect(
				settings.position.x,
				settings.position.y,
				settings.size.width,
				settings.size.height,
				borderColor
			);

			tft.setTextColor(THEME_TEXT, fillColor);
			tft.setTextDatum(MC_DATUM);
			tft.drawString(
				label.c_str(),
				settings.position.x + settings.size.width / 2,
				settings.position.y + settings.size.height / 2
			);

			hasChanged = false;
		}

		void press() {
			if(callback) callback();
		}

		std::string getLabel() const {
			return label;
		}

		void setLabel(const std::string &text) {
			label = text;
			markChanged();
		}
};
