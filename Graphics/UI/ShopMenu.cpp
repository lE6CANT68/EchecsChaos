#include "ShopMenu.h"

ShopMenu::ShopMenu() {
    // 1. Dimensions du grand panneau central
    d_panelWidth = 700;
    d_panelHeight = 400;
    d_panelX = 150; // À ajuster selon la taille de ta fenêtre (Config::Graphics...)
    d_panelY = 100;

    // 2. Dimensions et espacement des cartes
    d_cardWidth = 100;
    d_cardHeight = 150;
    d_spacing = 30; // L'espace vide entre chaque carte

    // 3. Point de départ de la première carte pour que ça soit bien centré
    d_startX = d_panelX + 40; 
    d_startY = d_panelY + 120;
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
            return i; // On a trouvé la carte cliquée !
        }
    }
    return std::nullopt;
}