#include <vector>

#include "../screens/screen.cpp"

#include "../screens/camera.cpp"
#include "../screens/settings/wifi.cpp"

#include "ui.cpp"

typedef enum {
	CAMERA_SCREEN,
	WIFI_SETTINGS_SCREEN,
	USER_SETTINGS
	// Add other screens here
} Screens;
class ScreensList: public UIComponent {
	private:
		Screen *_currentScreen;
	  Screens _currentScreenType;
		std::vector<Screens> _screenList;

	public:
		ScreensList(): UIComponent() {
			_visibilityChanged = true;
			_visible = false;
		}
		~ScreensList() {
			if (_currentScreen) {
				delete _currentScreen;
			}
		}

		void setCurrentScreen(Screens screenType) {
			if (_currentScreen)
				delete _currentScreen;

			switch(screenType) {
				case CAMERA_SCREEN:
					Serial.println("Setting current screen to CAMERA_SCREEN");
					_currentScreen = new CameraScreen();
					break;
				case WIFI_SETTINGS_SCREEN:
					Serial.println("Setting current screen to WIFI_SETTINGS_SCREEN");
					_currentScreen = new WifiSettingsScreen();
					break;
			}
			show();

			if(_currentScreen)
				_currentScreen->load();
		}

		void addScreenToList(Screens screenType) {
		  if(_currentScreen == nullptr) setCurrentScreen(screenType);
		  _screenList.push_back(screenType);
		}

		void render() {
			if(_visibilityChanged){
				tft.fillScreen(TFT_BLACK);
				_visibilityChanged = false;
			}

			if (_currentScreen) {
				if (!_currentScreen->isVisible()) {
					delete _currentScreen;

					if(_screenList.size()) {
            setCurrentScreen(*_screenList.rbegin());
					  return;
					}

					hide();
					return;
				}

				_currentScreen->render();
			}
		}

		void consumeKeys() {
			if (_currentScreen) {
				_currentScreen->consumeKeys();
			}
		}
};
