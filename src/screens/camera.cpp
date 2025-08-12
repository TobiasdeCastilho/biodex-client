#include "../classes/camera.cpp"
#include "screen.cpp"

class CameraScreen: public Screen {
	private:		
		Camera *_camera;

	public:
		CameraScreen(): Screen() {
			_camera = new Camera({{0,0}, {430,320}});
		}

		void render() {											
			_camera->render();
		}

		void consumeKeys() override {
			_camera->consumeKeys();
		}
}; 