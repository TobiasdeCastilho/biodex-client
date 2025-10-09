#include "../globals/buttons.h"

#include "../classes/camera.cpp"
#include "screen.cpp"

class CameraScreen: public Screen {
	private:
		Camera *_camera;

	public:
		CameraScreen(): Screen() {
			_camera = new Camera();
		}
		~CameraScreen() {
			delete _camera;
		}

		void render() {
			_camera->render();
		}

		void consumeKeys() override {
			if(btn_rn.consume()) hide();
			_camera->consumeKeys();
		}
};
