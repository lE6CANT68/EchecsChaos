#include "ShopMenu.h"
#include <algorithm>

ShopMenu::ShopMenu() {
    // Layout initial par défaut, sera recalculé en fonction de la fenêtre
    d_panelWidth = 700;
    d_panelHeight = 400;
    d_panelX = 50;
    d_panelY = 100;

    d_cardWidth = 100;
    d_cardHeight = 150;
    d_spacing = 30;

    d_startX = d_panelX + 40;
    d_startY = d_panelY + 120;
}

void ShopMenu::updateLayout(int screenWidth, int screenHeight, int numCards) {
    const int horizontalMargin = 40;
    const int verticalMargin = 40;
    const int panelTopMargin = 80;
    const int panelBottomMargin = 80;

    d_panelWidth = std::max(300, screenWidth - horizontalMargin * 2);
    if (d_panelWidth > 700) d_panelWidth = 700;

    d_panelHeight = std::max(260, screenHeight - panelTopMargin - panelBottomMargin);
    if (d_panelHeight > 420) d_panelHeight = 420;

    d_panelX = (screenWidth - d_panelWidth) / 2;
    d_panelY = (screenHeight - d_panelHeight) / 2;

    d_spacing = 20;
    int availableCardArea = d_panelWidth - 80;
    int targetCardWidth = (availableCardArea - (numCards - 1) * d_spacing) / std::max(1, numCards);
    d_cardWidth = std::clamp(targetCardWidth, 80, 120);
    d_cardHeight = static_cast<int>(d_cardWidth * 1.3f);

    int totalCardsWidth = numCards * d_cardWidth + (numCards - 1) * d_spacing;
    d_startX = d_panelX + std::max(40, (d_panelWidth - totalCardsWidth) / 2);
    d_startY = d_panelY + std::max(40, (d_panelHeight - d_cardHeight) / 2);
}

bool ShopMenu::isClickInsidePanel(int mouseX, int mouseY) const {
    return (mouseX >= d_panelX && mouseX <= d_panelX + d_panelWidth &&
            mouseY >= d_panelY && mouseY <= d_panelY + d_panelHeight);
}

std::optional<int> ShopMenu::getClickedCardIndex(int mouseX, int mouseY, int numCards) const {
    
    for (int i = 0; i < numCards; ++i) {
        int cardX = d_startX + i * (d_cardWidth + d_spacing);
        int cardY = d_startY;

        if (mouseX >= cardX && mouseX <= cardX + d_cardWidth &&
            mouseY >= cardY && mouseY <= cardY + d_cardHeight) {
            return i;
        }
    }
    return std::nullopt;
}