#include <sstream>

#include "ui.cpp"

class Dialog: public UIComponent {
	private:
		std::string _title;
		std::string _message;
		bool _hasChanged = false;

	public:
		Dialog(): UIComponent() {}

  	void render() override {
      if(!_hasChanged && !_visibilityChanged) return;

      _hasChanged = false;
      _visibilityChanged = false;

      // Quebrar mensagem em linhas
      std::vector<std::string> lines;
      std::istringstream messageStream(_message);
      std::string line;
      while (std::getline(messageStream, line)) lines.push_back(line);

      // Configura tamanhos de fonte
      tft.setTextSize(2);
      int titleHeight = tft.fontHeight(1);
      int titleWidth  = tft.textWidth(_title.c_str());

      tft.setTextSize(1);
      int lineHeight = tft.fontHeight(1) + 2;

      // Calcular largura máxima do texto
      int maxTextWidth = titleWidth;
      for (auto &line : lines) {
        int w = tft.textWidth(line.c_str());
        if (w > maxTextWidth) maxTextWidth = w;
      }

      // Margens internas
      int marginX = 20;
      int marginY = 15;

      // Dimensões da caixa
      int boxWidth = maxTextWidth + marginX * 2;
      int boxHeight = titleHeight + marginY * 2 + lines.size() * lineHeight + 5;

      // Posição centralizada
      int boxX = (TFT_WIDTH - boxWidth) / 2;
      int boxY = (TFT_HEIGHT - boxHeight) / 2;

      // Desenha fundo
      tft.fillRoundRect(boxX, boxY, boxWidth, boxHeight, 10, TFT_BLACK);
      tft.drawRoundRect(boxX, boxY, boxWidth, boxHeight, 10, TFT_WHITE);

      // Desenha título
      tft.setTextSize(2);
      int titleX = boxX + (boxWidth - titleWidth) / 2;
      int titleY = boxY + marginY;
      tft.drawString(_title.c_str(), titleX, titleY, 1);

      // Desenha linhas da mensagem
      tft.setTextSize(1);
      int y = titleY + titleHeight + 8;
      for (auto &line : lines) {
        int lineWidth = tft.textWidth(line.c_str());
        int x = boxX + (boxWidth - lineWidth) / 2;
        tft.drawString(line.c_str(), x, y, 1);
        y += lineHeight;
      }
  	}

		void consumeKeys() override {
			if(btn_sl.consume() || btn_rn.consume()) hide();
		}

		void open(const std::string& title, const std::string& message) {
			_title = title;
			_message = message;
			_hasChanged = true;
			_visibilityChanged = true;
		}
};
