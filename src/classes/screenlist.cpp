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
	
	public:
		ScreensList(): PrimitiveUI() {			
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

		void render() {
			if(_visibilityChanged){				
				tft.fillScreen(TFT_BLACK);
				_visibilityChanged = false;
			}			

			if (_currentScreen) {
				if (!_currentScreen->isVisible()) {
					delete _currentScreen;					
					_currentScreen = nullptr;					
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