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

	UIContainer *head = new UIContainer({{SCREEN_WIDTH,20},{0,0},THEME_ACCENT});
	head->addChild(
  	new UILabel({{SCREEN_WIDTH / 2,10},{0,10}, THEME_ACCENT},
  	  []() {
        std::string text = data.getWiFiId();
  	    return "WiFi: " + (!text.size() ? "Desconectado" : text);
  		}
  	)
	);
	head->addChild(
  	new UILabel({{SCREEN_WIDTH / 2,10},{SCREEN_WIDTH / 2,10}, THEME_ACCENT},
  	  []() {
        std::string text = data.getUsername();
  	    return "Usuario: " + (!text.size() ? "Indefinido" : text);
  		}
  	)
	);

	screen->addChild(head);
	screen->addChild(new UIButton({{80, 80}, {40, 140}, THEME_PRIMARY}, "Identificar", [](){
		 uiManager.setCurrentScreen(1);
	}));
	screen->addChild(new UIButton({{80, 80}, {200, 140}, THEME_PRIMARY}, "Usuario", [](){
		uiManager.setCurrentScreen(2);
	}));
	screen->addChild(new UIButton({{80, 80}, {360, 140}, THEME_PRIMARY}, "Wi-Fi", [](){
		uiManager.setCurrentScreen(2);
	}));

	return screen;
}
