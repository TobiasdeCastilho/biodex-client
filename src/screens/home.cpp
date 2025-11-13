#pragma once

#include <string>

#include "../globals/ui.h"
#include "../globals/manager.h"
#include "../globals/data.h"

#include "../ui/container.cpp"
#include "../ui/label.cpp"
#include "../ui/button.cpp"
#include "../ui/themes.h"

inline UIContainer* createHomeScreen() {
	UIComponentSettings mainSettings;
	mainSettings.position = {0, 0};
	mainSettings.size = {SCREEN_WIDTH, SCREEN_HEIGHT};
	mainSettings.color = THEME_BG;

	UIContainer *screen = new UIContainer(mainSettings);

	screen->addChild(
  	new UILabel({{100,10},{10,10}, THEME_BG},
  	  []() {
        std::string text = data.getWiFiId();
  	    return "WiFi: " + (text.size() ? "Desconectado" : text);
  		}
  	)
	);

	screen->addChild(new UIButton({{80, 80}, {40, 140}, THEME_PRIMARY}, "Click Me 1", [](){
		Serial.println("Button 1 pressed!");
	}));
	screen->addChild(new UIButton({{80, 80}, {200, 140}, THEME_PRIMARY}, "Click Me 3", [](){
		Serial.println("Button 3 pressed!");
	}));
	screen->addChild(new UIButton({{80, 80}, {360, 140}, THEME_PRIMARY}, "Wi-Fi\nSettings", [](){
		uiManager.setCurrentScreen(1);
	}));

	return screen;
}
