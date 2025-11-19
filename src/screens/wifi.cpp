#pragma once

#include <WiFi.h>

#include "../globals/data.h"
#include "../globals/ui.h"
#include "../globals/manager.h"

#include "../ui/container.cpp"
#include "../ui/button.cpp"
#include "../ui/themes.h"

#include "../connection/request.cpp"

inline UIContainer* createWiFiSettingsScreen() {
	UIComponentSettings mainSettings;
	mainSettings.position = {0, 0};
	mainSettings.size = {SCREEN_WIDTH, SCREEN_HEIGHT};
	mainSettings.color = THEME_BG;

	UIContainer *screen = new UIContainer(mainSettings);

	uiManager.toggleLoader(true);
	int networksAmount = WiFi.scanNetworks();
	uiManager.toggleLoader(false);

	int yOffset = 40;
	for(int i = 0; i < networksAmount; i++){
		std::string ssid = WiFi.SSID(i).c_str();

		UIComponentSettings btnSettings;
		btnSettings.position = {20, yOffset};
		btnSettings.size = {SCREEN_WIDTH - 40, 30};
		btnSettings.color = THEME_PRIMARY;

		UIButton *networkBtn = new UIButton(btnSettings, ssid, [i, ssid](){
			uiManager.useKeyboard([i, ssid](bool confirmed, std::string text, int textLength){
				if(confirmed){
					int maxTries = 20;

					WiFi.begin(ssid.c_str(), text.c_str());
					while (WiFi.status() != WL_CONNECTED && maxTries-- > 0) delay(500);
					if(WiFi.status() != WL_CONNECTED) return;

					data.setWiFiId(ssid.c_str());
					data.setWifiPassword(text);
					data.save();

					InitializeRequest initPayload;
					initPayload.user = "tobias";
					initPayload.password = "4002";

					request.requestInitialize(initPayload);
				}
			});
		});

		screen->addChild(networkBtn);
		yOffset += 40;
	}

	return screen;
}
