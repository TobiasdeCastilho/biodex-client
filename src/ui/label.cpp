#pragma once

#include <string>

#include "component.cpp"
#include "themes.h"

typedef std::function<std::string()> LabelGetTextCallback;

class UILabel : public UIComponent {
	private:
		std::string text;
		LabelGetTextCallback getTextCallback = NULL;

	public:
		UILabel(UIComponentSettings settings, const std::string &value)
		: UIComponent(settings), text(value) {
			setFocusable(false);
			type = LABEL;
		}

		UILabel(UIComponentSettings settings, LabelGetTextCallback callback)
		: UIComponent(settings), getTextCallback(callback) {
			setFocusable(false);
			type = LABEL;
		}

		void setText(const std::string &value) {
			text = value;
			markChanged();
		}

		void render() override {
			if(getTextCallback != NULL) {
			  std::string newText = getTextCallback();
				if(!newText.compare(text)) setText(newText);
			}

			if(!hasChanged) return;

			tft.fillRect(
				settings.position.x,
				settings.position.y,
				settings.size.width,
				settings.size.height,
				settings.color
			);

			tft.setTextColor(THEME_TEXT, settings.color);
			tft.setTextDatum(MC_DATUM);
			tft.drawString(
				text.c_str(),
				settings.position.x + settings.size.width / 2,
				settings.position.y + settings.size.height / 2
			);

			hasChanged = false;
		}
};
