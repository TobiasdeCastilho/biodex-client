#ifndef UI

#include <vector>

class PrimitiveUI {
	protected:
		bool _visible;
		bool _hasChanged;
		bool _visibilityChanged;

	public:
		PrimitiveUI(): _visible(false), _hasChanged(true), _visibilityChanged(true) {}
		virtual void render() = 0;

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
};

#define UI

#endif