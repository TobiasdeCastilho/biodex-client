#ifndef WIFI_SETTINGS

#include <WiFi.h>

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

			int networksAmount = WiFi.scanNetworks();
			
			Serial.println("Networks found: ");
			for (int i = 0; i < networksAmount; i++) {
				Serial.println(" - " + WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + "dBm)");
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

		void consumeKeys() {
			_options->consumeKeys();
		}
};

#define WIFI_SETTINGS

#endif