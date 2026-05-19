#pragma once
#include <optional>

class ShopMenu {
public:
    ShopMenu();
    void updateLayout(int screenWidth, int screenHeight, int numCardsInShop);
    std::optional<int> getClickedCardIndex(int mouseX, int mouseY, int numCardsInShop) const;
    bool isClickInsidePanel(int mouseX, int mouseY) const;
    
    int getPanelX() const { return d_panelX; }
    int getPanelY() const { return d_panelY; }
    int getPanelWidth() const { return d_panelWidth; }
    int getPanelHeight() const { return d_panelHeight; }
    
    int getStartX() const { return d_startX; }
    int getStartY() const { return d_startY; }
    int getCardWidth() const { return d_cardWidth; }
    int getCardHeight() const { return d_cardHeight; }
    int getSpacing() const { return d_spacing; }

private:
    int d_panelX, d_panelY, d_panelWidth, d_panelHeight;
    int d_startX, d_startY;
    int d_cardWidth, d_cardHeight;
    int d_spacing;
};