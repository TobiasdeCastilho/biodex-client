#ifndef SCREEN

#include <TFT_eSPI.h>

#include "../classes/ui.cpp"

class Screen: public UIComponent {
	public:
		Screen(): UIComponent() {
		}

		virtual void load() {
			show();
			return;
		}
};

#define SCREEN

#endif
