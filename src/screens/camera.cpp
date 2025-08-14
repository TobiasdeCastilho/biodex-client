#include "../globals/buttons.h"

#include "../classes/camera.cpp"
#include "screen.cpp"

class CameraScreen: public Screen {
	private:		
		Camera *_camera;

	public:
		CameraScreen(): Screen() {
			_camera = new Camera({{0,0}, {430,320}});
		}
		~CameraScreen() {
			delete _camera;
		}

		void render() {											
			_camera->render();
		}

		void consumeKeys() override {
			if(btn_rn.consume()){
				hide();				
				Serial.println("CameraScreen: Hide called");
			}
			
			_camera->consumeKeys();
		}
}; 