#pragma once
#include "raylib.h"
#include <vector>
#include "../Core/Base/Card.h"
#include "../Core/Base/Player.h"

class CardRenderer {
public:
    CardRenderer();
    void drawHands(const Player& currentPlayer, int mouseX, int mouseY) const;
    void drawCard(const Card& card, int x, int y, bool isHovered) const;
    int getClickedCardIndex(int numCards, int mouseX, int mouseY) const ;

private:
    static const int CARD_WIDTH = 100;
    static const int CARD_HEIGHT = 130;
    static const int CARD_SPACING = 15;
    static const int HOVER_LIFT = 10; 

    Color getRarityColor(CardRarity rarity) const;
    void drawGradientBackground(int x, int y, Color rarityColor) const;
};