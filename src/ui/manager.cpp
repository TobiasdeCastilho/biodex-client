#pragma once

#include <vector>

#include "../globals/hardware.h"

#include "container.cpp"
#include "button.cpp"
#include "loader.cpp"
#include "keyboard.cpp"

class UIManager {
private:
	std::vector<UIContainer*> screens;
	int currentScreenIndex = -1;

	UILoader loader;
	bool loaderActive = false;

	UIKeyboard keyboard;
	bool keyboardActive = false;

public:
	UIManager() {}

	void addScreen(UIContainer *screen) {
		screens.push_back(screen);
		if(currentScreenIndex == -1) currentScreenIndex = 0;

		if(!screens[currentScreenIndex]->getFocusedChild()){
			screens[currentScreenIndex]->focusNext();
		}
	}

	void setCurrentScreen(int index) {
		if(index >= 0 && index < (int)screens.size()){
			if(currentScreenIndex >=0 && currentScreenIndex < (int)screens.size()){
				UIContainer *old = screens[currentScreenIndex];
				UIComponent *focused = old->getFocusedChild();
				if(focused) focused->setFocus(false);
			}

			currentScreenIndex = index;
			UIContainer *current = screens[currentScreenIndex];
			current->markChanged();

			if(!current->getFocusedChild()){
				current->focusNext();
			}
		}
	}

	UIContainer* getCurrentScreen() {
		if(currentScreenIndex >= 0 && currentScreenIndex < (int)screens.size())
			return screens[currentScreenIndex];
		return nullptr;
	}

	void toggleLoader(bool show) {
		loaderActive = show;
		if(loaderActive){
			loader.show();
		} else {
			loader.hide();
			UIContainer *current = getCurrentScreen();
			if(current){
				current->markChanged();
			}
		}
	}

	// Usar teclado com callback
	void useKeyboard(KeyboardUserCallback cb) {
		keyboardActive = true;
		keyboard.use(cb);
	}

	void render() {
		if(loaderActive){
			if(loader.isVisible()) {
				loader.render();
				return;
			}
			toggleLoader(false);
		}

		if(keyboardActive){
			if(keyboard.isVisible()){
				keyboard.render();
				return;
			}
			keyboardActive = false;
			UIContainer *current = getCurrentScreen();
			if(current) current->markChanged();
		}

		UIContainer *current = getCurrentScreen();
		if(current) current->render();
	}

	void consumeKeys() {
		if(loaderActive) return;

		if(keyboardActive && keyboard.isVisible()) {
			keyboard.consumeKeys();
			return;
		}

		UIContainer *current = getCurrentScreen();
		if(!current) return;

		if(buttonLeft.consume()) current->focusPrev();
		if(buttonRight.consume()) current->focusNext();
		if(buttonUp.consume()) {}
		if(buttonDown.consume()) {}

		if(buttonSelect.consume()){
			UIComponent *focused = current->getFocusedChild();
			if(focused && focused->getType() == BUTTON){
				((UIButton*)focused)->press();
			}
		}

		if(buttonReturn.consume()) setCurrentScreen(0);
	}
};
