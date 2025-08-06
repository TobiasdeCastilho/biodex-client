#include "../screens/screen.cpp"

#include "../screens/camera.cpp"
#include "../screens/settings/wifi.cpp"

#include "ui.cpp"

typedef enum {
	CAMERA_SCREEN,
	WIFI_SETTINGS_SCREEN,
	// Add other screens here
} Screens;
class ScreensList: public PrimitiveUI {
	private:
		Screen *_currentScreen;
	  Screens _currentScreenType;
		TFT_eSPI *_tft;		
	
	public:
		ScreensList(TFT_eSPI *tft) {
			_tft = tft;
			_visibilityChanged = true;
			_visible = true;
		}
		~ScreensList() {
			if (_currentScreen) {
				delete _currentScreen;
			}
		}

		void setCurrentScreen(Screens screenType) {			
			if (!_currentScreen) {
				delete _currentScreen;
			}

			switch(screenType) {
				case CAMERA_SCREEN:
					Serial.println("Setting current screen to CAMERA_SCREEN");
					_currentScreen = new CameraScreen(_tft);		
					break;
				case WIFI_SETTINGS_SCREEN:
					Serial.println("Setting current screen to WIFI_SETTINGS_SCREEN");
					_currentScreen = new WifiSettingsScreen(_tft);
					break;
				default:
					break;
			}
		}

		void render() {
			if (_currentScreen) {
				_currentScreen->render();
			}			
		}
};