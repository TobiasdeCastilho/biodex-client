#ifndef SCREEN

#include <TFT_eSPI.h>

class Screen {
	protected:
		TFT_eSPI *_tft;

	public:
		Screen(TFT_eSPI *tft) {
			_tft = tft;
		}
		virtual void render() = 0;
};

#define SCREEN

#endif