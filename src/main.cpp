#include <Arduino.h>

#include <TFT_eSPI.h>
// Classes
#include "classes/button.cpp"
#include "classes/request.cpp"
#include "classes/camera.cpp"
#include "classes/options.cpp"
#include "classes/screenlist.cpp"
#include "classes/keyboard.cpp"

TFT_eSPI tft = TFT_eSPI();
Comunication comunication;

Button
	btn_up(19, "up"),
	btn_lf(20, "left"),
	btn_dw(21, "down"),
	btn_rt(47, "right"),
	btn_rn(48, "return"),
	btn_sl(45, "select");
Keyboard keyboard(&tft, &btn_up, &btn_dw, &btn_lf, &btn_rt, &btn_rn, &btn_sl);

OptionList optionList(
	&tft, 
	{0, 270},	
	{30, 30}
);
ScreensList screenList(&tft);

RenderList renderList;

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

	if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

	optionList.addOptions({
		Option([](bool active) { Serial.println("Option 1 selected"); }, "camera.bin", ""),
		Option([](bool active) { Serial.println("Option 2 selected"); }, "definicoes.bin", ""),
		Option([](bool active) { 
			screenList.setCurrentScreen(WIFI_SETTINGS_SCREEN); 
			optionList.hide();	
		}, "lista.bin", ""),
		Option([](bool active) { keyboard.show(); }, "teclado.bin", "")
	});

	tft.init();
	tft.setRotation(3);
	tft.invertDisplay(true);
	tft.fillScreen(TFT_BLACK);

	optionList.setInCenterX();

	renderList.add(&keyboard, 0);
	renderList.add(&screenList, 1);
	renderList.add(&optionList, 1);

	xTaskCreatePinnedToCore(
	  buttonsRead, 
	  "ButtonsRead", 
	  2048,        
	  NULL,        
	  1,           
	  NULL,        
	  0              
  );

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
	if(keyboard.isVisible()) {
		keyboard.consumeKeys();
	} else if(optionList.isVisible()){			
		if(btn_sl.consume())
			optionList.activeSelectedOption();	
		if(btn_lf.consume())
			optionList.decreaseSelectedOption();		
		if(btn_rt.consume())			
			optionList.increaseSelectedOption();
		// if(btn_rn.clicked()) {
		// 	optionList.deactivate();
		// }
	}
}