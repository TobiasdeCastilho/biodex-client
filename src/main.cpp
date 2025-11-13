#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"

#include "globals/ui.h"
#include "globals/manager.h"

#include "screens/home.cpp"
#include "screens/wifi.cpp"

void renderTask(void *param){
	tft.fillScreen(THEME_BG);
	while(true){
		uiManager.render();
		vTaskDelay(34 / portTICK_PERIOD_MS);
	}
}

void buttonsRead(void *param) {
	while (true) {
		buttonUp.read();
		buttonLeft.read();
		buttonDown.read();
		buttonRight.read();
		buttonReturn.read();
		buttonSelect.read();

		vTaskDelay(34 / portTICK_PERIOD_MS);
	}
}

void setup() {
	Serial.begin(115200);
	if(!LittleFS.begin() && (!LittleFS.exists("/data") || !LittleFS.mkdir("/data"))) return;

	tft.init();
	tft.setRotation(3);
	tft.invertDisplay(true);
	tft.fillScreen(THEME_BG);

	// Cria telas
	UIContainer *wifiScreen = createWiFiSettingsScreen();
	UIContainer *homeScreen = createHomeScreen();

	// Adiciona telas ao manager
	uiManager.addScreen(homeScreen);    // índice 0
	uiManager.addScreen(wifiScreen);    // índice 1
	uiManager.setCurrentScreen(0);

	// Cria tasks
	xTaskCreatePinnedToCore(
		renderTask,
		"Render",
		4096,
		NULL,
		1,
		NULL,
		1
	);

	xTaskCreatePinnedToCore(
	  buttonsRead,
	  "ButtonsRead",
	  2048,
	  NULL,
	  2,
	  NULL,
	  0
  );
}

void loop() {
	uiManager.consumeKeys();
	delay(20);
}
