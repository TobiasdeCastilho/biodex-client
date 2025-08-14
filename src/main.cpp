#include <Arduino.h>

#include "globals/tft.h"
#include "globals/buttons.h"
#include "globals/keyboard.h"
#include "globals/loaders.h"

// Classes
#include "classes/button.cpp"
#include "classes/request.cpp"
#include "classes/camera.cpp"
#include "classes/options.cpp"
#include "classes/screenlist.cpp"

Comunication comunication;
ControllerUI renderList;

OptionList optionList(	
	{0, 270},	
	{30, 30}
);
ScreensList screenList;

void buttonsRead(void *param) {
	while (true) {
		btn_up.read();
		btn_lf.read();
		btn_dw.read();
		btn_rt.read();
		btn_rn.read();
		btn_sl.read();

		vTaskDelay(10 / portTICK_PERIOD_MS); // Delay to avoid busy-waiting
	}
}

// void consumeKeys(void *param) {
// 	while (true) {
// 		renderList.consumeKeys();
// 		vTaskDelay(10 / portTICK_PERIOD_MS); // Delay to avoid busy-waiting
// 	}
// }

void render(void *param) {
	tft.fillScreen(TFT_BLACK);

	while(true) {
		if(keyboard.visibilityChanged()){			
			optionList.reset();
			screenList.reset();
		}
		if(screenList.visibilityChanged())
			optionList.reset();		

		renderList.render();

		vTaskDelay(34 / portTICK_PERIOD_MS); // Delay to avoid busy-waiting
	}
}

void setup(){	
	Serial.begin(115200);	

	Serial.println("Starting setup...");
	if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }	

	Serial.println("Initializing TFT display...");
	tft.init();
	tft.setRotation(3);
	tft.invertDisplay(true);
	tft.fillScreen(TFT_BLACK);
	Serial.println("TFT display initialized");

	Serial.println("Creating options list...");
	optionList.addOptions({
		Option([](bool active) { screenList.setCurrentScreen(CAMERA_SCREEN); }, "camera.bin", ""),
		Option([](bool active) { Serial.println("Option 2 selected"); }, "definicoes.bin", ""),
		Option([](bool active) { 
			screenList.setCurrentScreen(WIFI_SETTINGS_SCREEN); 			
		}, "lista.bin", ""),
		Option([](bool active) { keyboard.show(); }, "teclado.bin", "")
	});

	optionList.setInCenterX();	
	Serial.println("Options list created");

	Serial.println("Setting up render list...");
	renderList.add(&circleLoad);
	renderList.add(&loaderProgress);
	renderList.add(&keyboard);
	renderList.add(&screenList);
	renderList.add(&optionList);
	Serial.println("Render list setup complete");	

	Serial.println("Setup complete, starting tasks...");
	xTaskCreatePinnedToCore(
	  buttonsRead, 
	  "ButtonsRead", 
	  2048,        
	  NULL,        
	  2,           
	  NULL,        
	  0
  );

	// xTaskCreatePinnedToCore(
	//   consumeKeys, 
	//   "ConsumeKeys", 
	//   2048,        
	//   NULL,        
	//   1,           
	//   NULL,        
	//   0              
	// );

	xTaskCreatePinnedToCore(
	  render,
	  "render",
	  2048,
	  NULL,
	  1,
	  NULL,
	  1
  );	
}

void loop(){					
	renderList.consumeKeys();
}