#include "CardRenderer.h"

CardRenderer::CardRenderer() {}

Color CardRenderer::getRarityColor(CardRarity rarity) const {
    switch (rarity) {
        case CardRarity::Common:    return GRAY;
        case CardRarity::Rare:      return BLUE;
        case CardRarity::Epic:      return PURPLE;
        case CardRarity::Legendary: return GOLD;
        default:                    return WHITE;
    }
}

void CardRenderer::drawCard(const Card& card, int x, int y, bool isHovered) const {
    int drawY = isHovered ? y - HOVER_LIFT : y;
    Color rarityColor = getRarityColor(card.getRarity());
    
    DrawRectangle(x, drawY, CARD_WIDTH, CARD_HEIGHT, DARKGRAY);
    DrawRectangleLinesEx({(float)x, (float)drawY, (float)CARD_WIDTH, (float)CARD_HEIGHT}, 3, rarityColor);

    DrawRectangle(x + 5, drawY + 5, CARD_WIDTH - 10, 25, rarityColor);
    DrawText(card.getName().c_str(), x + 10, drawY + 12, 10, WHITE);

    DrawCircle(x + CARD_WIDTH - 12, drawY + 12, 10, GOLD);
    DrawText(TextFormat("%d", card.getCost()), x + CARD_WIDTH - 16, drawY + 7, 12, BLACK);

    DrawRectangle(x + 10, drawY + 40, CARD_WIDTH - 20, 60, BLACK);
    DrawText("Chaos Effect", x + 15, drawY + 110, 10, LIGHTGRAY);
}

void CardRenderer::drawHands(const std::vector<Player>& players, int mouseX, int mouseY) const {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    const Card* hoveredCard = nullptr;
    int tooltipX = 0;
    int tooltipY = 0;

    for (int p = 0; p < players.size(); ++p) {
        const auto& hand = players[p].getHand();
        int n = hand.size();
        if (n == 0) continue;
        
        int totalWidth = (n * CARD_WIDTH) + ((n - 1) * CARD_SPACING);
        int startX = (screenWidth - totalWidth) / 2;
        int baseY = (p == 0) ? (screenHeight - CARD_HEIGHT - 20) : 20;

        for (int i = 0; i < n; ++i) {
            int cardX = startX + i * (CARD_WIDTH + CARD_SPACING);
            bool isHovered = (mouseX >= cardX && mouseX <= cardX + CARD_WIDTH &&
                              mouseY >= baseY && mouseY <= baseY + CARD_HEIGHT);

            drawCard(*hand[i], cardX, baseY, isHovered);

            
            if (isHovered) {
                hoveredCard = hand[i].get();
                tooltipX = mouseX;
                tooltipY = mouseY;
            }
        }
    }

    
    if (hoveredCard != nullptr) {

        DrawRectangle(tooltipX + 15, tooltipY + 15, 200, 60, Fade(BLACK, 0.9f));
        DrawRectangleLines(tooltipX + 15, tooltipY + 15, 200, 60, GOLD);
        
        // Nom de la carte
        DrawText(hoveredCard->getName().c_str(), tooltipX + 25, tooltipY + 20, 15, GOLD);

        DrawText(hoveredCard->getDescription().c_str(), tooltipX + 25, tooltipY + 45, 10, WHITE);
    }
}
int CardRenderer::getClickedCardIndex(int playerIndex, int numCards, int mouseX, int mouseY) const {

    if (numCards == 0) return -1; 

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int totalWidth = (numCards * CARD_WIDTH) + ((numCards - 1) * CARD_SPACING);
    int startX = (screenWidth - totalWidth) / 2;

    
    int baseY = (playerIndex == 0) ? (screenHeight - CARD_HEIGHT - 20) : 20;

    for (int i = 0; i < numCards; ++i) {
        int cardX = startX + i * (CARD_WIDTH + CARD_SPACING);
        if (mouseX >= cardX && mouseX <= cardX + CARD_WIDTH &&
            mouseY >= baseY && mouseY <= baseY + CARD_HEIGHT) {
            
            return i; 
        }
    }
    return -1; 
}