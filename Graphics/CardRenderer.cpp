#include "CardRenderer.h"

CardRenderer::CardRenderer() {}

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
    // Si la carte est survolée, elle monte légèrement
    int drawY = isHovered ? y - HOVER_LIFT : y;
    Color rarityColor = getRarityColor(card.getRarity());
    
    Rectangle cardRect = { (float)x, (float)drawY, (float)CARD_WIDTH, (float)CARD_HEIGHT };

    // 1. L'OMBRE PORTÉE (Indispensable pour détacher les cartes du fond)
    Rectangle shadowRect = { cardRect.x + 5, cardRect.y + 5, cardRect.width, cardRect.height };
    DrawRectangleRounded(shadowRect, 0.15f, 10, { 0, 0, 0, 150 });

    // 2. FOND DÉGRADÉ (Du sombre vers la couleur de rareté)
    drawGradientBackground(x, drawY, rarityColor);
    
    // 3. BORDURE ARRONDIE
    DrawRectangleRoundedLines(cardRect, 0.15f, 10, rarityColor);
    // 4. BANNIÈRE DU TITRE (Avec un peu de transparence pour le style)
    Rectangle headerRect = { cardRect.x + 5, cardRect.y + 5, cardRect.width - 10, 25 };
    DrawRectangleRounded(headerRect, 0.2f, 10, Fade(rarityColor, 0.6f));
    DrawText(card.getName().c_str(), x + 12, drawY + 12, 10, WHITE);

    // 5. CRISTAL DE COÛT (Effet "gemme" avec 2 cercles)
    DrawCircle(x + CARD_WIDTH - 12, drawY + 12, 13, { 180, 130, 0, 255 }); // Contour sombre
    DrawCircle(x + CARD_WIDTH - 12, drawY + 12, 10, { 255, 220, 50, 255 }); // Intérieur brillant
    DrawText(TextFormat("%d", card.getCost()), x + CARD_WIDTH - 16, drawY + 7, 12, BLACK);

    // 6. ZONE DE TEXTE DE L'EFFET (Un encart assombri)
    Rectangle effectRect = { cardRect.x + 10, cardRect.y + 40, cardRect.width - 20, 60 };
    DrawRectangleRounded(effectRect, 0.1f, 10, { 20, 15, 25, 255 }); 
    DrawText("Chaos Effect", x + 15, drawY + 110, 10, { 150, 150, 150, 255 });
}

void CardRenderer::drawHands(const Player& currentPlayer, int mouseX, int mouseY) const {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    const Card* hoveredCard = nullptr;
    int tooltipX = 0;
    int tooltipY = 0;

    const auto& hand = currentPlayer.getHand();
    int n = hand.size();
    if (n == 0) return; 
    
    int totalWidth = (n * CARD_WIDTH) + ((n - 1) * CARD_SPACING);
    int startX = (screenWidth - totalWidth) / 2;
    int baseY = screenHeight - CARD_HEIGHT - 20; 

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
    int bandHeight = CARD_HEIGHT / 10;
    for (int i = 0; i < 10; ++i) {
        // Interpoler la couleur entre darkColor et rarityColor
        float t = (float)i / 10.0f;
        Color bandColor = {
            (unsigned char)(darkColor.r + (rarityColor.r - darkColor.r) * t),
            (unsigned char)(darkColor.g + (rarityColor.g - darkColor.g) * t),
            (unsigned char)(darkColor.b + (rarityColor.b - darkColor.b) * t),
            255
        };
        
        Rectangle band = { (float)x, (float)(y + i * bandHeight), (float)CARD_WIDTH, (float)bandHeight };
        DrawRectangleRounded(band, 0.15f, 10, bandColor);
    }
}

int CardRenderer::getClickedCardIndex(int numCards, int mouseX, int mouseY) const {
    if (numCards == 0) return -1; 

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int totalWidth = (numCards * CARD_WIDTH) + ((numCards - 1) * CARD_SPACING);
    int startX = (screenWidth - totalWidth) / 2;
    int baseY = screenHeight - CARD_HEIGHT - 20;

    for (int i = 0; i < numCards; ++i) {
        int cardX = startX + i * (CARD_WIDTH + CARD_SPACING);
        if (mouseX >= cardX && mouseX <= cardX + CARD_WIDTH &&
            mouseY >= baseY && mouseY <= baseY + CARD_HEIGHT) {
            return i; 
        }
    }
    return -1; 
}