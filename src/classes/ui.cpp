#ifndef UI

#include <vector>

#include "../globals/buttons.h"
#include "../globals/tft.h"

#include "../types/ui.cpp"

class PrimitiveUI {
	protected:
		bool _visible, _hasChanged,_visibilityChanged;		
		Definition _definition;

	public:		
		PrimitiveUI(): _visible(false), _hasChanged(true), _visibilityChanged(true) {}		
		virtual void render() = 0;
		virtual void consumeKeys() = 0;

		void hide() {
			_visible = false;
			_hasChanged = true;
			_visibilityChanged = true;
		}

		void show() {
			_visible = true;
			_hasChanged = true;
			_visibilityChanged = true;
		}

		bool isVisible() {
			return _visible;
		}

		bool visibilityChanged() {			
			return _visibilityChanged;
		}

		void reset() {
			if(!_visible) return;
			_hasChanged = true;
			_visibilityChanged = true;
		}
};

typedef struct {
	PrimitiveUI *ui;
	int priority;
} RenderItem;
class RenderList {
	private: 
		std::vector<RenderItem> _items;		
	public:
		void add(PrimitiveUI *item, int priority) {
			_items.push_back({item, priority});
		}

		void render() {
			int lastPriority = -1;
			for (auto &item : _items) {				
				if(lastPriority != -1 && item.priority > lastPriority) return; // Stop rendering if priority is lower than last rendered item								
				if (item.ui->isVisible() || item.ui->visibilityChanged()) {

					if(item.ui->isVisible()) lastPriority = item.priority;
					item.ui->render();
				}
			}
		}

		void consumeKeys() {
			for (auto &item : _items) {			
				if (item.ui->isVisible()) {
					item.ui->consumeKeys();
					return; 
				}
			}
		}
};

#define UI

#endif