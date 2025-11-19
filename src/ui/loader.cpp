#pragma once

#include <math.h>

#include "../globals/ui.h"

#include "component.cpp"
#include "themes.h"

#define CIRCLE_LOAD_PARTS 12

class UILoader : public UIComponent {
  private:
  	int part = 0;
  	unsigned long lastUpdate = 0;
  	bool visibleFlag = false;

  public:
  	UILoader()
  	: UIComponent({{SCREEN_WIDTH, SCREEN_HEIGHT},{0,0},THEME_BG}) {
  		setFocusable(false);
  		type = COMPONENT;
  	}

  	void show() { visibleFlag = true; markChanged(); }
  	void hide() { visibleFlag = false; markChanged(); }
  	bool isVisible() { return visibleFlag; }

  	void render() override {
  		if(!visibleFlag) return;

  		unsigned long currentMillis = millis();
  		if(currentMillis - lastUpdate > 100) markChanged();

  		if(!isChanged()) return;

      // If is the first render
  		if(!lastUpdate) tft.fillRect(settings.position.x, settings.position.y, settings.size.width, settings.size.height, settings.color);

  		hasChanged = false;
  		lastUpdate = currentMillis;

  		const int totalParts = CIRCLE_LOAD_PARTS;
  		const int centerX = settings.position.x + settings.size.width / 2;
  		const int centerY = settings.position.y + settings.size.height / 2;
  		const float radiusOrbit = 40.0f;
  		const float maxCircleRadius = 8.0f;


  		for(int i = totalParts; i > 0; i--){
  			float angleDeg = ((i + part) % totalParts) * (360.0f / totalParts);
  			float angleRad = angleDeg * M_PI / 180.0f;

  			int x = centerX + radiusOrbit * cos(angleRad);
  			int y = centerY + radiusOrbit * sin(angleRad);

  			int brightness = 255 - (i * (200 / totalParts));
  			uint16_t color = tft.color565(brightness, brightness, brightness);

  			tft.fillCircle(x, y, maxCircleRadius, color);
  		}

  		part = (part + 1) % totalParts;
   }
};
