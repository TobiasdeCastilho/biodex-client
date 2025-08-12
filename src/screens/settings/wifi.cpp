#ifndef WIFI_SETTINGS

#include <WiFi.h>

#include "../../globals/tft.h"
#include "../../globals/keyboard.h"
#include "../../globals/loaders.h"

#include "../screen.cpp"
#include "../../types/ui.cpp"
#include "../../classes/options.cpp"

class WifiSettingsScreen: public Screen {
	private:
		OptionList * _options;

	public:
    WifiSettingsScreen(): Screen() {		
			_options = new OptionList({0, 0}, {460, 30});
			_options->setListType(VERTICAL);			
		}

    void render(){
			if(_visibilityChanged) {
				tft.fillScreen(TFT_BLACK);
				_options->reset();
				_visibilityChanged = false;
			}

			_options->render();
		}

		void load() {
			circleLoad.show();
			int networksAmount = WiFi.scanNetworks();								

			const int progressInclusion = 50 / networksAmount;
			for (int i = 0; i < networksAmount; i++) {				
				_options->addOptions({
					Option([i](bool active) {
						keyboard.use([i](bool confirmed, std::string text, int textLength) {
							if(confirmed) {					            
								WiFi.begin(WiFi.SSID(i).c_str(), text.c_str());								
								int maxTries = 20;
								while (WiFi.status() != WL_CONNECTED && maxTries-- > 0) delay(500);
								return;
							}
						});
					}, "", WiFi.SSID(i).c_str())

				});
				delay(100);
			}

			circleLoad.hide();
			show();
		}

		void consumeKeys() {
			_options->consumeKeys();

			if(btn_rn.consume())
				hide();
		}
};

#define WIFI_SETTINGS

#endif