#include <math.h>

#include "ui.cpp"

class LoaderProgress: public PrimitiveUI {
	private:
		int _progress = 0;
		bool _loading = false;
	
	public:
		LoaderProgress(): PrimitiveUI() {}

		void increaseProgress(int progress) {
			_progress = progress;
			if(_progress > 100) _progress = 100;			
			show();
		}
		
		void completeLoading() {			
			_progress = 0;
			hide();
		}

		void render() override {
			if(!_hasChanged && !_visibilityChanged) return;

			_hasChanged = false;

			if(_visibilityChanged){
				tft.fillScreen(TFT_BLACK);
				_visibilityChanged = false;

				if(_visible)
					tft.drawRoundRect(TFT_WIDTH / 2 - 71, TFT_HEIGHT / 2 - 11, 142, 22, 10, TFT_WHITE);
			}

			if(!_visible)
				return;

			tft.fillRoundRect(TFT_WIDTH / 2 - 70, TFT_HEIGHT / 2 - 10, 140 * _progress / 100, 20, 10, TFT_GREEN);			
		}
};

#define CIRCLE_LOAD_PARTS 12
class CircleLoad: public PrimitiveUI {
	private:
		int _part = CIRCLE_LOAD_PARTS;
		unsigned long _lastUpdate = 0;	

	public:
		CircleLoad(): PrimitiveUI() {}

		void render() override {
			unsigned long currentMillis = millis();
	    if(currentMillis - _lastUpdate > 100) _hasChanged = true;

	    if(!_visibilityChanged && !_hasChanged) return;
	    
			_hasChanged = false;

			if(_visibilityChanged){
        tft.fillScreen(TFT_BLACK);			
        _visibilityChanged = false;
	    }

			if(!_visible) return;			    

	    const int totalParts = 12;
	    const int centerX = TFT_WIDTH / 2;
	    const int centerY = TFT_HEIGHT / 2;
	    const float radiusOrbit = 40.0f;
	    const float maxCircleRadius = 8.0f;

	    for (int i = totalParts; i > 0; i--) {
        float angleDeg = ((i + _part) % totalParts) * (360.0f / totalParts);
        float angleRad = angleDeg * M_PI / 180.0f;

        int x = centerX + radiusOrbit * cos(angleRad);
        int y = centerY + radiusOrbit * sin(angleRad);
        
        int brightness = 255 - (i * (200 / totalParts));
        uint16_t color = tft.color565(brightness, brightness, brightness);
        
        float pulse = (i * (maxCircleRadius - 2) / (float)totalParts);

				tft.fillCircle(x, y, maxCircleRadius, TFT_BLACK);
        tft.fillCircle(x, y, pulse, color);
	    }

	    // _part = (_part - 1 + totalParts) % totalParts;
			_part = (_part + 1) % totalParts; // Increment part for next render
	    _lastUpdate = millis();
		}
};