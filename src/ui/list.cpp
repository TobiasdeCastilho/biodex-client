#pragma once

#include <vector>

#include "../globals/hardware.h"

#include "component.cpp"
#include "themes.h"

enum UIListOrientation {
	VERTICAL,
	HORIZONTAL
};

class UIList : public UIComponent {
	private:
		std::vector<UIComponent*> items;
		UIListOrientation orientation;
		int scrollOffset = 0;
		int itemSpacing = 4;

	public:
		UIList(UIComponentSettings settings, UIListOrientation orientation = VERTICAL) : UIComponent(settings), orientation(orientation) {
			type = LIST;
			setFocusable(true);
		}

		void addItem(UIComponent *item) {
			items.push_back(item);
			calculateItemPositions();
			markChanged();
		}

		std::vector<UIComponent*> getItems() {
			return items;
		}

		void scroll(int delta) {
			int maxScroll = getMaxScroll();
			scrollOffset += delta;
			if(scrollOffset < 0) scrollOffset = 0;
			if(scrollOffset > maxScroll) scrollOffset = maxScroll;
			markChanged();
		}

		void render() override {
			if(!hasChanged) return;

			tft.fillRect(
				settings.position.x,
				settings.position.y,
				settings.size.width,
				settings.size.height,
				settings.color
			);

			for (auto *item : items) {
				if(isVisible(item)) item->render();
			}

			hasChanged = false;
		}

		void focusNext() {
			if(items.empty()) return;
			int current = getFocusedIndex();
			int next = current + 1;
			if(next >= (int)items.size()) next = 0;

			if(current >= 0) items[current]->setFocus(false);
			items[next]->setFocus(true);

			ensureVisible(next);
			markChanged();
		}

		void focusPrevious() {
			if(items.empty()) return;
			int current = getFocusedIndex();
			int prev = current - 1;
			if(prev < 0) prev = items.size() - 1;

			if(current >= 0) items[current]->setFocus(false);
			items[prev]->setFocus(true);

			ensureVisible(prev);
			markChanged();
		}

	private:
		void calculateItemPositions() {
			int pos = 0;
			for (auto *item : items) {
				if (orientation == VERTICAL) {
					item->setPosition({
						settings.position.x,
						settings.position.y + pos
					});
					pos += item->getSize().height + itemSpacing;
				} else {
					item->setPosition({
						settings.position.x + pos,
						settings.position.y
					});
					pos += item->getSize().width + itemSpacing;
				}
			}
		}

		int getFocusedIndex() {
			for (int i = 0; i < (int)items.size(); i++) {
				if(items[i]->hasFocus()) return i;
			}
			return -1;
		}

		bool isVisible(UIComponent *item) {
			int itemStart = orientation == VERTICAL ? item->getPosition().y : item->getPosition().x;
			int itemEnd = itemStart + (orientation == VERTICAL ? item->getSize().height : item->getSize().width);
			int viewStart = orientation == VERTICAL ? settings.position.y + scrollOffset : settings.position.x + scrollOffset;
			int viewEnd = viewStart + (orientation == VERTICAL ? settings.size.height : settings.size.width);
			return itemEnd >= viewStart && itemStart <= viewEnd;
		}

		void ensureVisible(int index) {
			if(index < 0 || index >= (int)items.size()) return;

			auto *item = items[index];
			int itemStart = orientation == VERTICAL ? item->getPosition().y : item->getPosition().x;
			int itemEnd = itemStart + (orientation == VERTICAL ? item->getSize().height : item->getSize().width);

			int viewStart = orientation == VERTICAL ? settings.position.y + scrollOffset : settings.position.x + scrollOffset;
			int viewEnd = viewStart + (orientation == VERTICAL ? settings.size.height : settings.size.width);

			if(itemEnd > viewEnd) scroll(itemEnd - viewEnd);
			else if(itemStart < viewStart) scroll(itemStart - viewStart);
		}

		int getMaxScroll() {
			if(items.empty()) return 0;
			int lastPos = orientation == VERTICAL
				? items.back()->getPosition().y + items.back()->getSize().height
				: items.back()->getPosition().x + items.back()->getSize().width;
			int viewSize = orientation == VERTICAL ? settings.size.height : settings.size.width;
			int totalSize = lastPos - (orientation == VERTICAL ? settings.position.y : settings.position.x);
			return totalSize > viewSize ? totalSize - viewSize : 0;
		}

		bool consumeKeys() override {
		  switch(orientation) {
				case(VERTICAL): {
				  if(buttonDown.consume()) focusNext();
					if(buttonUp.consume()) focusPrevious();
					buttonLeft.consume();
					buttonRight.consume();
					break;
				}
				default: {
				  if(buttonLeft.consume()) focusNext();
   			  if(buttonRight.consume()) focusPrevious();
          buttonDown.consume();
					buttonUp.consume();
     			break;
    		}
			}
		}
};
