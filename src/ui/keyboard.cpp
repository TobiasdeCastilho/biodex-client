#pragma once

#include <vector>
#include <string>
#include <functional>

#include "../globals/ui.h"
#include "../globals/hardware.h"

#include "component.cpp"
#include "container.cpp"
#include "button.cpp"
#include "label.cpp"
#include "themes.h"

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 10
#define KEY_SIZE 48

typedef std::function<void(bool confirmed, std::string text, int textLength)> KeyboardUserCallback;

class UIKeyboard : public UIContainer {
private:
	std::vector<std::vector<char>> keyMap = {
		{'1','2','3','4','5','6','7','8','9','0'},
		{'Q','W','E','R','T','Y','U','I','O','P'},
		{'A','S','D','F','G','H','J','K','L',':'},
		{'Z','X','C','V','B','N','M',',','.',';'}
	};

	char input[64] = {0};
	int inputLength = 0;
	int keyX = 0, keyY = 0;

	bool visible = false;
	bool uppercase = true;

	UILabel *inputLabel;
	UIButton *keyButtons[5][KEYBOARD_COLS] = {nullptr};

	std::vector<std::function<void()>> specialKeyCallbacks;
	std::vector<UIButton*> specialButtons;

	KeyboardUserCallback callback = nullptr;

public:
	UIKeyboard() : UIContainer({{0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT}, THEME_BG}) {
		setFocusable(false);
		type = COMPONENT;
		visible = false;

		UIComponentSettings labelSettings;
		labelSettings.position = {10, 10};
		labelSettings.size = {SCREEN_WIDTH - 20, 40};
		labelSettings.color = THEME_BG;
		inputLabel = new UILabel(labelSettings, "");
		addChild(inputLabel);

		int startY = 80;
		for (int y = 0; y < KEYBOARD_ROWS; y++) {
			for (int x = 0; x < KEYBOARD_COLS; x++) {
				char c = keyMap[y][x];
				UIComponentSettings keySettings;
				keySettings.position = {x * KEY_SIZE, startY + y * KEY_SIZE};
				keySettings.size = {KEY_SIZE, KEY_SIZE};
				keySettings.color = THEME_ACCENT;

				UIButton *btn = new UIButton(keySettings, std::string(1, c), [this, x, y]() {
					this->onKeyPress(x, y);
				});
				addChild(btn);
				keyButtons[y][x] = btn;
			}
		}

		createSpecialKeys(startY + KEYBOARD_ROWS * KEY_SIZE);

		// inicializa foco inicial
		keyButtons[0][0]->setFocus(true);
	}

	void show() { visible = true; markChanged(); }
	void hide() { visible = false; }
	bool isVisible() { return visible; }

	void render() override {
		if (!visible) return;
		UIContainer::render();
	}

	void consumeKeys() {
		if (!visible) return;

		int lastX = keyX;
		int lastY = keyY;

		if (buttonLeft.consume()) keyX--;
		if (buttonRight.consume()) keyX++;
		if (buttonUp.consume()) keyY--;
		if (buttonDown.consume()) keyY++;

		// limite vertical
		if (keyY < 0) keyY = 4;
		else if (keyY > 4) keyY = 0;

		// limite horizontal
		if (keyY < 4) {
			if (keyX < 0) keyX = KEYBOARD_COLS - 1;
			else if (keyX >= KEYBOARD_COLS) keyX = 0;
		} else {
			if (keyX < 0) keyX = (int)specialButtons.size() - 1;
			else if (keyX >= (int)specialButtons.size()) keyX = 0;
		}

		if (lastX != keyX || lastY != keyY) {
			if (lastY < 4 && keyButtons[lastY][lastX])
				keyButtons[lastY][lastX]->setFocus(false);
			else if (lastY == 4 && lastX < (int)specialButtons.size())
				specialButtons[lastX]->setFocus(false);

			if (keyY < 4 && keyButtons[keyY][keyX])
				keyButtons[keyY][keyX]->setFocus(true);
			else if (keyY == 4 && keyX < (int)specialButtons.size())
				specialButtons[keyX]->setFocus(true);

			markChanged();
		}

		if (buttonSelect.consume())
			onKeyPress(keyX, keyY);
	}

	void use(KeyboardUserCallback cb) {
		callback = cb;
		resetInput();
		show();
	}

private:
	void createSpecialKeys(int y) {
		std::vector<std::pair<std::string, std::function<void()>>> specials = {
			{"SHIFT", [this]() { uppercase = !uppercase; updateKeysCase(); }},
			{"SPACE", [this]() { appendChar(' '); }},
			{"BACK", [this]() {
				if (inputLength > 0) {
					inputLength--;
					input[inputLength] = '\0';
					updateLabel();
				}
			}},
			{"ENTER", [this]() {
				if (callback) callback(true, std::string(input), inputLength);
				resetInput();
			}}
		};

		int xPos = 0;
		for (auto &spec : specials) {
			UIComponentSettings s;
			s.position = {xPos, y};
			s.size = {KEY_SIZE * 2, KEY_SIZE};
			s.color = THEME_ACCENT;

			UIButton *btn = new UIButton(s, spec.first, spec.second);
			addChild(btn);
			specialButtons.push_back(btn);
			specialKeyCallbacks.push_back(spec.second);

			xPos += KEY_SIZE * 2;
		}
	}

	void onKeyPress(int x, int y) {
		if (y == 4) {
			if (x < (int)specialKeyCallbacks.size())
				specialKeyCallbacks[x]();
			return;
		}

		char key = keyMap[y][x];
		if (!uppercase) key = tolower(key);
		appendChar(key);
	}

	void appendChar(char c) {
		if (inputLength >= 63) return;
		input[inputLength++] = c;
		input[inputLength] = '\0';
		updateLabel();
	}

	void updateLabel() {
		inputLabel->setText(std::string(input));
	}

	void updateKeysCase() {
		for (int y = 0; y < KEYBOARD_ROWS; y++) {
			for (int x = 0; x < KEYBOARD_COLS; x++) {
				char c = keyMap[y][x];
				if (!uppercase) c = tolower(c);
				keyButtons[y][x]->setLabel(std::string(1, c));
			}
		}
	}

	void resetInput() {
		memset(input, 0, sizeof(input));
		inputLength = 0;
		updateLabel();

		// reinicia foco
		if (keyButtons[keyY][keyX])
			keyButtons[keyY][keyX]->setFocus(false);
		keyX = keyY = 0;
		keyButtons[0][0]->setFocus(true);

		hide();
	}
};
