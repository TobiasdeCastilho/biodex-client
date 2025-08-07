// #include "../classes/camera.cpp"
#include "screen.cpp"

class CameraScreen: public Screen {
	private:		
		// Camera _camera;

	public:
		CameraScreen(): Screen() {

		}

		void render() override {											
			// _camera.render(0, 0, *tft);
		}

		void consumeKeys() override {
			// Handle key inputs specific to the camera screen here
		}
}; 