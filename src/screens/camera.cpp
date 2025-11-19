#pragma once

#include "../globals/manager.h"
#include "../globals/hardware.h"
#include "../globals/ui.h"

#include "../ui/container.cpp"
#include "../ui/themes.h"
#include "../ui/camera.cpp"

#include "../hardware/audio.cpp"

inline UIContainer* createCameraScreen() {
  UIComponentSettings mainSettings;
  mainSettings.position = {0, 0};
  mainSettings.size = {SCREEN_WIDTH, SCREEN_HEIGHT};
  mainSettings.color = THEME_BG;
  UIContainer* screen = new UIContainer(mainSettings);

  UIContainer *identification = new UIContainer({{SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT}, {SCREEN_WIDTH / 2 + 10, 0}, THEME_BG});
  UILabel *popularName = new UILabel({{SCREEN_WIDTH / 2 - 10, 20},{SCREEN_WIDTH / 2 + 10, 25}, THEME_BG},
 	  []() {
      if(!request.lastIdentification.popularName.size()) return "";
 	    return request.lastIdentification.popularName.c_str();
		}
 	);
 	UILabel *animalName = new UILabel({{SCREEN_WIDTH / 2 - 10, 10},{SCREEN_WIDTH / 2 + 10, 5}, THEME_BG},
 	  []() {
      if(!request.lastIdentification.name.size()) return "";
 	    return request.lastIdentification.name.c_str();
    }
 	);
 	UILabel *animalDescription = new UILabel({{SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT - 20},{SCREEN_WIDTH / 2 + 10, 30}, THEME_BG},
 	  []() {
      if(!request.lastIdentification.description.size()) return "";
 	    return request.lastIdentification.description.c_str();
		}
 	);
  identification->addChild(popularName);
  identification->addChild(animalName);
	identification->addChild(animalDescription);
	identification->setVisible(false);

	screen->addChild(identification);

  UICamera* camera = new UICamera();
  camera->setPosition({SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 88});
  camera->setCallback([camera, screen, identification](camera_fb_t* photo){
    if(!photo) return;

    uiManager.toggleLoader(true);

    camera->setPosition({0, SCREEN_HEIGHT / 2 - 88});
    camera->forceRender();
    identification->setVisible(true);

    auto identifyTask = [](void* arg){
      camera_fb_t* fb = (camera_fb_t*)arg;

      IdentifyRequest identifyReq;
      identifyReq.latitude = 0.0;
      identifyReq.longitude = 0.0;

      request.requestIdentify(identifyReq, fb);
      uiManager.toggleLoader(false);

      delay(100);
      play_raw_pwm(request.lastIdentification.audio,request.lastIdentification.audioSize);

      vTaskDelete(nullptr);
    };

    xTaskCreatePinnedToCore(
      identifyTask,
      "identifyTask",
      41060,
      photo,
      1,
      nullptr,
      1
    );
  });

  screen->setOnPress([camera, screen, identification]() {
  	if (!identification->isVisible() && buttonReturn.check()) {
      buttonReturn.consume();

  		identification->setVisible(true);
  		camera->setPosition({SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 88});
      camera->resetPhoto();
  		screen->markChanged();
  	}

  	return true;
  });

  screen->addChild(camera);
  return screen;
}
