#ifndef WIFI_SETTINGS

#include <WiFi.h>

#include "../screen.cpp"
#include "../../types/screen.cpp"
#include "../../classes/options.cpp"

class WifiSettingsScreen: public Screen {
	private:
		OptionList * _options;

	public:
    WifiSettingsScreen(TFT_eSPI *tft): Screen(tft) {		
			_options = new OptionList(_tft, {0, 0}, {460, 30});
			_options->setListType(VERTICAL);

			int networksAmount = WiFi.scanNetworks();

			Serial.println("Networks found: " + String(networksAmount));

			for (int i = 0; i < networksAmount; i++) {
				_options->addOptions({
					Option([i](bool active) {
						if(active) {
							WiFi.begin(WiFi.SSID(i).c_str());
						} else {
							WiFi.disconnect();
						}
					}, "", WiFi.SSID(i).c_str())
				});
			}

		}

    void render(){
			_options->render();
		}
};

#define WIFI_SETTINGS

#endif