#pragma once

#include <vector>
#include <string>

#include "../globals/ui.h"

#include "themes.h"

typedef struct {
	int x;
	int y;
} UIPosition;

typedef struct {
	int width;
	int height;
} UISize;

typedef struct {
	UISize size;
	UIPosition position;
	int color;
} UIComponentSettings;

typedef enum {
	COMPONENT,
	CONTAINER,
	BUTTON,
	LABEL,
	LIST
} UIComponentType;

class UIComponent {
	protected:
		bool hasChanged = true;
		bool focusable = false;
		bool focused = false;
		UIComponentSettings settings;
		UIComponentType type;

	public:
		UIComponent(UIComponentSettings componentSettings) {
			settings = componentSettings;
			type = COMPONENT;
		}

		UIComponent(int componentColor) {
			settings = {
				{0, 0},
				{SCREEN_WIDTH, SCREEN_HEIGHT},
				componentColor
			};
			type = COMPONENT;
		}

		virtual void render() {}
		virtual bool isContainer() { return false; }

		virtual void markChanged() { hasChanged = true; }
		bool isChanged() const { return hasChanged; }

		void setFocusable(bool f) { focusable = f; }
		bool isFocusable() const { return focusable; }

		void setFocus(bool f) {
			if (focusable) {
				focused = f;
				markChanged();
			}
		}

		bool hasFocus() const { return focused; }

		UIComponentType getType() const { return type; }

		UISize getSize() const { return settings.size; }
		UIPosition getPosition() const { return settings.position; }

		void setPosition(UIPosition pos) {
			settings.position = pos;
			markChanged();
		}

		void setSize(UISize sz) {
			settings.size = sz;
			markChanged();
		}
};
