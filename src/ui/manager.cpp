#pragma once

#include <vector>
#include <functional>

#include "../globals/hardware.h"

#include "container.cpp"
#include "button.cpp"
#include "loader.cpp"
#include "keyboard.cpp"

class UIManager {
  private:
    std::vector<std::function<UIContainer*()>> screenFactories;
    std::vector<UIContainer*> screens;

    UILoader* loader = nullptr;
    bool loaderActive = false;

    UIKeyboard* keyboard = nullptr;
    bool keyboardActive = false;

  public:
    UIManager() {}

    ~UIManager() {
      if(loader) { delete loader; loader = nullptr; }
      if(keyboard) { delete keyboard; keyboard = nullptr; }

      for(auto screen : screens) {
        if(screen) delete screen;
      }
    }

    void addScreen(std::function<UIContainer*()> screenFactory) {
      screenFactories.push_back(screenFactory);
      screens.push_back(nullptr);
    }

    void setCurrentScreen(int index) {
      if (index < 0 || index >= (int)screenFactories.size()) return;

      UIContainer* oldScreen = getCurrentScreen();
      if(oldScreen) {
        delete oldScreen;
        screens.pop_back();
      }

      screens.push_back(screenFactories[index]());

      UIContainer* current = getCurrentScreen();
      if(!current) return;
      current->markChanged();
      if (!current->getFocusedChild()) current->focusNext();
    }

    UIContainer* getCurrentScreen() {
      if (!screens.size()) return nullptr;
      return screens[screens.size() - 1];
    }

    void toggleLoader(bool show) {
      loaderActive = show;

      if(loaderActive) {
        if(!loader) loader = new UILoader();
        loader->show();
      } else {
        if(loader) {
          loader->hide();
          delete loader;
          loader = nullptr;
        }

        UIContainer* current = getCurrentScreen();
        if (current) current->markChanged();
      }
    }

    void useKeyboard(KeyboardUserCallback cb) {
      keyboardActive = true;
      if(!keyboard) keyboard = new UIKeyboard();
      keyboard->use(cb);
    }

    void render() {
      if (loaderActive) {
        if(loader && loader->isVisible()) { loader->render(); return; }
        toggleLoader(false);
      }

      if (keyboardActive) {
        if(keyboard && keyboard->isVisible()) { keyboard->render(); return; }
        keyboardActive = false;
        if(keyboard) { delete keyboard; keyboard = nullptr; }

        UIContainer* current = getCurrentScreen();
        if (current) current->markChanged();
      }

      UIContainer* current = getCurrentScreen();
      if (current) current->render();
    }

    void consumeKeys() {
      if (loaderActive) return;

      if (keyboardActive && keyboard && keyboard->isVisible()) { keyboard->consumeKeys(); return; }

      if (buttonReturn.consume()) setCurrentScreen(0);

      UIContainer* current = getCurrentScreen();
      if (!current) return;
      if (!current->consumeKeys()) return;

      if (buttonLeft.consume()) current->focusPrev();
      if (buttonRight.consume()) current->focusNext();
      if (buttonUp.consume()) {}
      if (buttonDown.consume()) {}

      if (buttonSelect.consume()) {
        UIComponent* focused = current->getFocusedChild();
        if (focused && focused->getType() == BUTTON) ((UIButton*)focused)->press();
      }
    }
};
