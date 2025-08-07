#ifndef SCREEN

#include <TFT_eSPI.h>

#include "../classes/ui.cpp"

class Screen: public PrimitiveUI {
	public:
		Screen(): PrimitiveUI() {			
		}
};

#define SCREEN

#endif