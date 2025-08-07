#include <Arduino.h>

#include "globals/tft.h"
#include "globals/buttons.h"

Button btn_up(19, "up");
Button btn_lf(20, "left");
Button btn_dw(21, "down");
Button btn_rt(47, "right");
Button btn_rn(48, "return");
Button btn_sl(45, "select");

// Classes
#include "classes/button.cpp"
#include "classes/request.cpp"
#include "classes/camera.cpp"
#include "classes/options.cpp"
#include "classes/screenlist.cpp"
#include "classes/keyboard.cpp"

Comunication comunication;
RenderList renderList;

TFT_eSPI tft = TFT_eSPI();
Keyboard keyboard;
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

void renderUI(void *param) {
	tft.fillScreen(TFT_BLACK);

	while(true) {
		if(keyboard.visibilityChanged()){			
			optionList.reset();
			screenList.reset();
		}

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
		Option([](bool active) { Serial.println("Option 1 selected"); }, "camera.bin", ""),
		Option([](bool active) { Serial.println("Option 2 selected"); }, "definicoes.bin", ""),
		Option([](bool active) { 
			screenList.setCurrentScreen(WIFI_SETTINGS_SCREEN); 
			optionList.hide();	
		}, "lista.bin", ""),
		Option([](bool active) { keyboard.show(); }, "teclado.bin", "")
	});

	optionList.setInCenterX();	
	Serial.println("Options list created");

	Serial.println("Setting up render list...");
	renderList.add(&keyboard, 0);
	renderList.add(&optionList, 1);
	renderList.add(&screenList, 2);
	Serial.println("Render list setup complete");	

	Serial.println("Setup complete, starting tasks...");
	xTaskCreatePinnedToCore(
	  buttonsRead, 
	  "ButtonsRead", 
	  2048,        
	  NULL,        
	  1,           
	  NULL,        
	  1             
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
	  renderUI,
	  "RenderUI",
	  2048,       
	  NULL,       
	  1,          
	  NULL,       
	  0
  );	
}

void loop(){				
	renderList.consumeKeys();
}