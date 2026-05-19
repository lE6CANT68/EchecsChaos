#include "CardRenderer.h"

CardRenderer::CardRenderer() {}

void CardRenderer::updateCardDimensions(int boardWidth) {
    if (boardWidth == 10) {
        // Plateau agrandi : cartes plus petites
        d_cardWidth = 70;
        d_cardHeight = 90;
        d_cardSpacing = 10;
        d_hoverLift = 8;
    } else {
        // Plateau normal (8x8)
        d_cardWidth = 100;
        d_cardHeight = 130;
        d_cardSpacing = 15;
        d_hoverLift = 10;
    }
}

Color CardRenderer::getRarityColor(CardRarity rarity) const {
    switch (rarity) {
        // Des couleurs "flashy" et chaotiques faites sur mesure !
        case CardRarity::Common:    return { 170, 180, 180, 255 }; 
        case CardRarity::Rare:      return { 50, 180, 255, 255 }; 
        case CardRarity::Epic:      return { 220, 50, 255, 255 };  
        case CardRarity::Legendary: return { 255, 190, 20, 255 };  
        default:                    return WHITE;
    }
}

void CardRenderer::drawCard(const Card& card, int x, int y, bool isHovered) const {
    int drawY = isHovered ? y - d_hoverLift : y;
    Color rarityColor = getRarityColor(card.getRarity());
    
    DrawRectangle(x, drawY, d_cardWidth, d_cardHeight, DARKGRAY);
    DrawRectangleLinesEx({(float)x, (float)drawY, (float)d_cardWidth, (float)d_cardHeight}, 3, rarityColor);

    DrawRectangle(x + 5, drawY + 5, d_cardWidth - 10, 25, rarityColor);
    DrawText(card.getName().c_str(), x + 10, drawY + 12, 10, WHITE);

    DrawCircle(x + d_cardWidth - 12, drawY + 12, 10, GOLD);
    DrawText(TextFormat("%d", card.getCost()), x + d_cardWidth - 16, drawY + 7, 12, BLACK);

    DrawRectangle(x + 10, drawY + 40, d_cardWidth - 20, 60, BLACK);
    DrawText("Chaos Effect", x + 15, drawY + 110, 10, LIGHTGRAY);
}

void CardRenderer::drawHands(const Player& currentPlayer, int mouseX, int mouseY) const {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    const Card* hoveredCard = nullptr;
    int tooltipX = 0;
    int tooltipY = 0;

    const auto& hand = currentPlayer.getHand();
        int n = hand.size();
    if (n > 0) {
        
        int totalWidth = (n * d_cardWidth) + ((n - 1) * d_cardSpacing);
        int startX = (screenWidth - totalWidth) / 2;
        int baseY = screenHeight - d_cardHeight - 20;

        for (int i = 0; i < n; ++i) {
            int cardX = startX + i * (d_cardWidth + d_cardSpacing);
            bool isHovered = (mouseX >= cardX && mouseX <= cardX + d_cardWidth &&
                              mouseY >= baseY && mouseY <= baseY + d_cardHeight);

            drawCard(*hand[i], cardX, baseY, isHovered);

            
            if (isHovered) {
                hoveredCard = hand[i].get();
                tooltipX = mouseX;
                tooltipY = mouseY;
            }
        }
    }
    if (hoveredCard != nullptr) {
        Color rarityColor = getRarityColor(hoveredCard->getRarity());
        Rectangle tooltipRect = { (float)tooltipX + 15, (float)tooltipY + 15, 200, 70 };
        
        DrawRectangleRounded(tooltipRect, 0.1f, 10, Fade(BLACK, 0.9f));
        DrawRectangleRoundedLines(tooltipRect, 0.1f, 10, rarityColor);
        DrawText(hoveredCard->getName().c_str(), tooltipX + 25, tooltipY + 25, 15, rarityColor);
        DrawText(hoveredCard->getDescription().c_str(), tooltipX + 25, tooltipY + 50, 10, WHITE);
    }
}

void CardRenderer::drawGradientBackground(int x, int y, Color rarityColor) const {
    // Crée un dégradé du noir (en bas) vers la couleur de rareté (en haut)
    Color darkColor = { (unsigned char)(rarityColor.r / 3), (unsigned char)(rarityColor.g / 3), (unsigned char)(rarityColor.b / 3), 255 };
    
    // Diviser en 10 bandes horizontales pour un dégradé smooth
    int bandHeight = d_cardHeight / 10;
    for (int i = 0; i < 10; ++i) {
        // Interpoler la couleur entre darkColor et rarityColor
        float t = (float)i / 10.0f;
        Color bandColor = {
            (unsigned char)(darkColor.r + (rarityColor.r - darkColor.r) * t),
            (unsigned char)(darkColor.g + (rarityColor.g - darkColor.g) * t),
            (unsigned char)(darkColor.b + (rarityColor.b - darkColor.b) * t),
            255
        };
        
        Rectangle band = { (float)x, (float)(y + i * bandHeight), (float)d_cardWidth, (float)bandHeight };
        DrawRectangleRounded(band, 0.15f, 10, bandColor);
    }
}
int CardRenderer::getClickedCardIndex(int playerIndex, int numCards, int mouseX, int mouseY) const {
    if (numCards == 0) return -1; 

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int totalWidth = (numCards * d_cardWidth) + ((numCards - 1) * d_cardSpacing);
    int startX = (screenWidth - totalWidth) / 2;

    
    int baseY = (playerIndex == 0) ? (screenHeight - d_cardHeight - 20) : 20;

    for (int i = 0; i < numCards; ++i) {
        int cardX = startX + i * (d_cardWidth + d_cardSpacing);
        if (mouseX >= cardX && mouseX <= cardX + d_cardWidth &&
            mouseY >= baseY && mouseY <= baseY + d_cardHeight) {
            
            return i; 
        }
    }
    return -1; 
}