#ifndef UI

#include <vector>

#include "../globals/buttons.h"
#include "../globals/tft.h"

#include "../types/ui.cpp"

class PrimitiveUI {
	protected:
		bool _visible, _hasChanged, _visibilityChanged;		
		Definition _definition;					

	public:		
		PrimitiveUI(): _visible(false), _hasChanged(true), _visibilityChanged(true) {}				
		virtual void render() = 0;

		virtual void consumeKeys() {
			return;
		};	

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

class ControllerUI {
	private: 
		std::vector<PrimitiveUI*> _items;		
	public:
		void add(PrimitiveUI* item) {
			_items.push_back(item);
		}

		void render() {
			bool shouldReset = false;

			for(int i = 0; i < _items.size(); i++) {
				if (_items[i]->isVisible() || _items[i]->visibilityChanged()) {
					_items[i]->render();					
					if(_items[i]->isVisible()){
						if(shouldReset)
							_items[i]->reset();
						return;
					}
					
					if(_items[i]->visibilityChanged())
						shouldReset = true;					
				}
			}
		}

		void consumeKeys() {
			for (auto *item : _items) {			
				if (item->isVisible()) {
					item->consumeKeys();
					return; 
				}
			}
		}
};

#define UI

#endif