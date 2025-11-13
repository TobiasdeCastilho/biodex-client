#pragma once

#include <vector>

#include "component.cpp"
#include "themes.h"

class UIContainer : public UIComponent {
	private:
		std::vector<UIComponent*> childrens;
		int padding = 5;
		int focusIndex = -1;

	public:
		UIContainer(UIComponentSettings containerSettings, std::vector<std::pair<UISize, UIComponentType>> childDefinitions = {})
		: UIComponent(containerSettings) {
		  if(!settings.color) settings.color = THEME_BG;
			type = CONTAINER;
			addChildren(childDefinitions);
		}

		void markChanged() override {
		  hasChanged = true;
			for(const auto &child : childrens) child->markChanged();
		}

		void addChildren(std::vector<std::pair<UISize, UIComponentType>> childDefinitions){
			int cursorX = padding;
			int cursorY = padding;
			int rowHeight = 0;

			for(const auto &def : childDefinitions){
				UISize childSize = def.first;
				UIComponentType childType = def.second;

				if(cursorX + childSize.width + padding > settings.size.width){
					cursorX = padding;
					cursorY += rowHeight + padding;
					rowHeight = 0;
				}

				UIComponentSettings childSettings = {
					childSize,
					{settings.position.x + cursorX, settings.position.y + cursorY},
					THEME_ACCENT
				};

				UIComponent *child;
				if(childType == CONTAINER){
					child = new UIContainer(childSettings);
				} else {
					child = new UIComponent(childSettings);
					child->setFocusable(true);
				}

				childrens.push_back(child);
				cursorX += childSize.width + padding;
				if(childSize.height > rowHeight) rowHeight = childSize.height;
			}

			for(size_t i=0;i<childrens.size();i++){
				if(childrens[i]->isFocusable()){
					focusIndex = i;
					childrens[i]->setFocus(true);
					break;
				}
			}
		}

		void addChild(UIComponent *child){
			childrens.push_back(child);
			if(focusIndex == -1 && child->isFocusable()){
				focusIndex = childrens.size()-1;
				child->setFocus(true);
			}
		}

		virtual bool isContainer(){ return true; }

		void render() override{
			if(hasChanged){
				tft.fillRect(settings.position.x, settings.position.y, settings.size.width, settings.size.height, settings.color);
				hasChanged = false;
			}

			for(auto &child : childrens){
				child->render();
			}
		}

		void focusNext(){
			if(childrens.empty()) return;
			if(focusIndex != -1) childrens[focusIndex]->setFocus(false);

			size_t start = (focusIndex + 1) % childrens.size();
			size_t i = start;
			do{
				if(childrens[i]->isFocusable()){
					focusIndex = i;
					childrens[i]->setFocus(true);
					break;
				}
				i = (i + 1) % childrens.size();
			} while(i != start);
		}

		void focusPrev(){
			if(childrens.empty()) return;
			if(focusIndex != -1) childrens[focusIndex]->setFocus(false);

			size_t i = (focusIndex == 0) ? childrens.size()-1 : focusIndex-1;
			size_t start = i;
			do{
				if(childrens[i]->isFocusable()){
					focusIndex = i;
					childrens[i]->setFocus(true);
					break;
				}
				i = (i == 0) ? childrens.size()-1 : i-1;
			} while(i != start);
		}

		UIComponent* getFocusedChild(){
			if(focusIndex >= 0 && focusIndex < childrens.size()) return childrens[focusIndex];
			return nullptr;
		}

		std::vector<UIComponent*> getChildren(){ return childrens; }
};
