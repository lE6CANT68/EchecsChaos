#pragma once
#include "raylib.h"
#include <vector>
#include "../Core/Base/Card.h"
#include "../Core/Base/Player.h"

class CardRenderer {
public:
    CardRenderer();
    void drawHands(const std::vector<Player>& players, int mouseX, int mouseY) const;
    void drawCard(const Card& card, int x, int y, bool isHovered) const;
    int getClickedCardIndex(int playerIndex, int numCards, int mouseX, int mouseY) const ;
    void updateCardDimensions(int boardWidth);

private:
    int d_cardWidth = 100;
    int d_cardHeight = 130;
    int d_cardSpacing = 15;
    int d_hoverLift = 10;

    Color getRarityColor(CardRarity rarity) const;
};