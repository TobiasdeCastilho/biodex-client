// #include "../classes/camera.cpp"
#include "screen.cpp"

class CameraScreen: public Screen {
	private:		
		// Camera _camera;

	public:
		CameraScreen(TFT_eSPI *tft): Screen(tft) {

		}

		void render() override {											
			// _camera.render(0, 0, *_tft);
		}
}; 