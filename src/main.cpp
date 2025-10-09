#include <Arduino.h>

#include "globals/tft.h"
#include "globals/buttons.h"
#include "globals/keyboard.h"
#include "globals/loaders.h"
#include "globals/dialog.h"

// Classes
#include "classes/button.cpp"
#include "classes/request.cpp"
#include "classes/camera.cpp"
#include "classes/options.cpp"
#include "classes/screenlist.cpp"

/** User Setup
#define ST7796_DRIVER
#define USE_HSPI_PORT

#define TFT_CS   1
#define TFT_RST  2
#define TFT_DC   42
#define TFT_MOSI 41
#define TFT_SCLK 40
#define TFT_MISO 39

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_GFXFF

#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY  20000000
**/

Comunication comunication;
UIController renderList;

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

		vTaskDelay(34 / portTICK_PERIOD_MS); // Delay to avoid busy-waiting
	}
}

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

void setupInfos() {
  circleLoad.show();

  // Verify current Wi-Fi connection
  std::string wifiId = data.getWiFiId(), wifiPassword = data.getWiFiPassword();
  if(wifiId != "" && wifiPassword != "") {
    int tries = 10;

    WiFi.begin(wifiId.c_str(),wifiPassword.c_str());
    while (WiFi.status() != WL_CONNECTED && tries-- > 0) delay(500);
  }
  if(WiFi.status() != WL_CONNECTED)
    screenList.addScreenToList(WIFI_SETTINGS_SCREEN);

  // Verify user data
  if(data.getUserId() == "")
    screenList.addScreenToList(USER_SETTINGS);

  circleLoad.hide();
}

void setup(){
	Serial.begin(115200);
	if(!LittleFS.begin() && (!LittleFS.exists("/data") || !LittleFS.mkdir("/data"))) return;

	data.load();

	tft.init();
	tft.setRotation(3);
	tft.invertDisplay(true);
	tft.fillScreen(TFT_BLACK);

	optionList.addOptions({
		Option([](bool active) {
			if(WiFi.status() != WL_CONNECTED){
				dialog.open("Não conectado", "É necessário conectar à rede Wi-Fi para iniciar\no processo de identificação");
				return;
			}
			screenList.setCurrentScreen(CAMERA_SCREEN);
		}, "camera.bin", ""),
		Option([](bool active) { }, "definicoes.bin", ""),
		Option([](bool active) {
			screenList.setCurrentScreen(WIFI_SETTINGS_SCREEN);
		}, "lista.bin", ""),
		Option([](bool active) { keyboard.show(); }, "teclado.bin", "")
	});
	optionList.setInCenterX();

	renderList.add(&dialog);
	renderList.add(&circleLoad);
	renderList.add(&loaderProgress);
	renderList.add(&keyboard);
	renderList.add(&screenList);
	renderList.add(&optionList);

	xTaskCreatePinnedToCore(
	  buttonsRead,
	  "ButtonsRead",
	  2048,
	  NULL,
	  2,
	  NULL,
	  0
  );

	xTaskCreatePinnedToCore(
	  render,
	  "render",
	  2048,
	  NULL,
	  1,
	  NULL,
	  1
  );

  setupInfos();
}

void loop(){
	renderList.consumeKeys();
}
