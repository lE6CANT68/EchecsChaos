#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(float cellSize) : d_cellSize(cellSize), d_offsetX(Config::Graphics::DEFAULT_OFFSETX), d_offsetY(Config::Graphics::DEFAULT_OFFSETY) {
    d_drawers[PieceType::Pawn] = std::make_unique<PawnDrawer>();
    d_drawers[PieceType::Bishop] = std::make_unique<BishopDrawer>();
    d_drawers[PieceType::Knight] = std::make_unique<KnightDrawer>();
    d_drawers[PieceType::Rook] = std::make_unique<RookDrawer>();
    d_drawers[PieceType::King] = std::make_unique<KingDrawer>();
    d_drawers[PieceType::Queen] = std::make_unique<QueenDrawer>();
    d_drawers[PieceType::Idiot] = std::make_unique<IdiotDrawer>();
    d_drawers[PieceType::PionDebile] = std::make_unique<PionDebileDrawer>();
    d_drawers[PieceType::Duck] = std::make_unique<DuckDrawer>();
}

void Renderer::updateDimensionsForBoard(const Board& board) {
    int currentWidth = board.getWidth();
    int currentHeight = board.getHeight();
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    if (!d_initializedDimensions && screenWidth > 0 && screenHeight > 0) {
        d_initializedDimensions = true;
        d_lastBoardWidth = currentWidth;
        
        int availableHeight = screenHeight - 250;
        int availableWidth = screenWidth - 80;
        
        float maxCellSize = std::min(
            availableWidth / (float)currentWidth,
            availableHeight / (float)currentHeight
        );
        
        d_cellSize = maxCellSize;
        d_offsetX = (screenWidth - (currentWidth * d_cellSize)) / 2.0f;
        d_offsetY = (screenHeight - (currentHeight * d_cellSize)) / 2.0f; 
    }
    if (currentWidth != d_lastBoardWidth && screenWidth > 0 && screenHeight > 0) {
        d_lastBoardWidth = currentWidth;
        int availableHeight = screenHeight - 250; 
        int availableWidth = screenWidth - 80;    
        float maxCellSize = std::min(
            availableWidth / (float)currentWidth,
            availableHeight / (float)currentHeight
        );
        d_cellSize = maxCellSize;
        d_offsetX = (screenWidth - (currentWidth * d_cellSize)) / 2.0f;
        d_offsetY = (screenHeight - (currentHeight * d_cellSize)) / 2.0f; 
        if (currentWidth == 10) {
            d_cardRenderer.updateCardDimensions(10);
        } else {
            d_cardRenderer.updateCardDimensions(8);
        }
    }
}

void Renderer::draw(const Board& board, Position selectedPos, const std::vector<Position>& validMoves,PieceColor currentColor,int bottomScore, int topScore,Position kingInCheckPos) {
    updateDimensionsForBoard(board);
    drawBoard(board, currentColor);
    drawBoardBorder(board);
    drawPortals(board);
    drawTileHighlights(selectedPos, kingInCheckPos, board, currentColor); 
    drawPieces(board, currentColor);
    drawMoveHints(board, validMoves, currentColor);   
    drawFogLayer(board, selectedPos, validMoves, currentColor);
    int scoreWidth = 140;
    int scoreHeight = 30;
    int scorePosX = GetScreenWidth() - 170;
    int scoreTopY = 20;
    int scoreBottomY = GetScreenHeight() - 50;

    if (currentColor == PieceColor::White) {
        DrawRectangle(scorePosX, scoreTopY, scoreWidth, scoreHeight, BLACK);
        DrawRectangleLines(scorePosX, scoreTopY, scoreWidth, scoreHeight, GRAY);
        DrawText(TextFormat("Noir : %d", topScore), scorePosX + 10, scoreTopY + 5, 20, WHITE);

        DrawRectangle(scorePosX, scoreBottomY, scoreWidth, scoreHeight, RAYWHITE);
        DrawRectangleLines(scorePosX, scoreBottomY, scoreWidth, scoreHeight, GOLD);
        DrawText(TextFormat("Blanc : %d", bottomScore), scorePosX + 10, scoreBottomY + 5, 20, BLACK);
    } else {
        DrawRectangle(scorePosX, scoreTopY, scoreWidth, scoreHeight, RAYWHITE);
        DrawRectangleLines(scorePosX, scoreTopY, scoreWidth, scoreHeight, DARKGRAY);
        DrawText(TextFormat("Blanc : %d", topScore), scorePosX + 10, scoreTopY + 5, 20, BLACK);

        DrawRectangle(scorePosX, scoreBottomY, scoreWidth, scoreHeight, BLACK);
        DrawRectangleLines(scorePosX, scoreBottomY, scoreWidth, scoreHeight, GOLD);
        DrawText(TextFormat("Noir : %d", bottomScore), scorePosX + 10, scoreBottomY + 5, 20, WHITE);
    }
}
void Renderer::drawBoard(const Board& board, PieceColor currentColor) {
    for (int x = 0; x < board.getWidth(); ++x) {
        for (int y = 0; y < board.getHeight(); ++y) {
            int displayY = (currentColor == PieceColor::Black) ? getFlippedY(y, board.getHeight()) : y;
            Color color = ((x + y) % 2 == 0) ? LIGHTGRAY : GRAY;
            
            float posX = d_offsetX + (x * d_cellSize);
            float posY = d_offsetY + (displayY * d_cellSize);

            
            DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, color);

            
            drawTileDecoration(board.getTile({x, y}).getType(), posX, posY);
        }
    }
}

void Renderer::drawBoardBorder(const Board& board) {
    float startX = d_offsetX;
    float startY = d_offsetY;
    float endX = d_offsetX + (board.getWidth() * d_cellSize);
    float endY = d_offsetY + (board.getHeight() * d_cellSize);
    float borderThickness = 4.0f;
    
    // Haut
    DrawLineEx({startX, startY}, {endX, startY}, borderThickness, DARKGRAY);
    // Bas
    DrawLineEx({startX, endY}, {endX, endY}, borderThickness, DARKGRAY);
    // Gauche
    DrawLineEx({startX, startY}, {startX, endY}, borderThickness, DARKGRAY);
    // Droite
    DrawLineEx({endX, startY}, {endX, endY}, borderThickness, DARKGRAY);
}

void Renderer::drawTileDecoration(TileType type, float posX, float posY) const {
    switch (type) {
        case TileType::Blocked:
            DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, Fade(DARKGRAY, 0.8f));
            DrawLineEx({posX, posY}, {posX + d_cellSize, posY + d_cellSize}, 5, BLACK);
            DrawLineEx({posX + d_cellSize, posY}, {posX, posY + d_cellSize}, 5, BLACK);
            break;
            
        case TileType::Lava: 
        {   
            
            DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, {220, 50, 0, 255});

            int pseudoRandom1 = (int)(posX * 13 + posY * 7) % 15;
            int pseudoRandom2 = (int)(posX * 3 + posY * 11) % 15;
            
            DrawCircle((int)posX + 15 + pseudoRandom1, (int)posY + 15 + pseudoRandom2, 8, {255, 140, 0, 200});
            DrawCircle((int)posX + 45 - pseudoRandom2, (int)posY + 35 + pseudoRandom1, 5, {255, 200, 0, 180});
            DrawCircle((int)posX + 25 + pseudoRandom2, (int)posY + 55 - pseudoRandom1, 6, {255, 100, 0, 220});

            DrawRectangleLinesEx({posX, posY, d_cellSize, d_cellSize}, 2, {50, 10, 0, 200});
            break;
            
        }   
            
        case TileType::Ice:
            // DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, Fade(SKYBLUE, 0.5f));
            break;
            
        case TileType::Normal:
        default:
            break;
            
        case TileType::Frozen:
            DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, Fade(SKYBLUE, 0.5f));
            DrawRectangleLinesEx({posX + 2, posY + 2, d_cellSize - 4, d_cellSize - 4}, 3, Fade(WHITE, 0.8f));
            break;
    }
}

void Renderer::drawPieces(const Board& board, PieceColor currentColor) {
    for (int x = 0; x < board.getWidth(); ++x) {
        for (int y = 0; y < board.getHeight(); ++y) {
            Position pos = {x, y};
            const Tile& tile = board.getTile(pos);

            if (tile.hasPiece()) {
                Piece* currentPiece = tile.getPiece();
                int displayY = (currentColor == PieceColor::Black) ? getFlippedY(y, board.getHeight()) : y;
                float centerX = d_offsetX + (x * d_cellSize) + (d_cellSize / 2.0f);
                float centerY = d_offsetY + (displayY * d_cellSize) + (d_cellSize / 2.0f);
                Color pieceColor = (currentPiece->getColor() == PieceColor::White) ? WHITE : BLACK;

                auto it = d_drawers.find(currentPiece->getType());
                if (it != d_drawers.end()) {
                    it->second->draw(centerX, centerY, pieceColor, d_cellSize);
                } 
                else {
                    DrawCircle((int)centerX, (int)centerY, FALLBACK_PIECE_RADIUS, RED); 
                }
            }
        }
    }
}
void Renderer::drawTileHighlights(Position selectedPos, Position kingInCheckPos, const Board& board, PieceColor currentColor) {
    if (selectedPos.x != -1 && selectedPos.y != -1) {
        int displayY = (currentColor == PieceColor::Black) ? getFlippedY(selectedPos.y, board.getHeight()) : selectedPos.y;
        float posX = d_offsetX + (selectedPos.x * d_cellSize);
        float posY = d_offsetY + (displayY * d_cellSize);
        DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, Config::Colors::HIGHLIGHT_COLOR); 
    }
    if (kingInCheckPos.x != -1 && kingInCheckPos.y != -1) {
        int displayY = (currentColor == PieceColor::Black) ? getFlippedY(kingInCheckPos.y, board.getHeight()) : kingInCheckPos.y;
        float posX = d_offsetX + (kingInCheckPos.x * d_cellSize);
        float posY = d_offsetY + (displayY * d_cellSize);
        DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, Config::Colors::CHECK_HIGHLIGHT_COLOR); 
    }
}
void Renderer::drawMoveHints(const Board& board, const std::vector<Position>& validMoves, PieceColor currentColor) {
    for (const Position& move : validMoves) {
        int displayY = (currentColor == PieceColor::Black) ? getFlippedY(move.y, board.getHeight()) : move.y;
        float centerX = d_offsetX + (move.x * d_cellSize) + (d_cellSize / 2.0f);
        float centerY = d_offsetY + (displayY * d_cellSize) + (d_cellSize / 2.0f);
        if (board.getTile(move).hasPiece() || move == board.getEnPassantTarget()) {
            DrawRing({centerX, centerY}, d_cellSize/2.0f - 8.0f, d_cellSize/2.0f - 2.0f, 0, 360, 32, {0, 0, 0, 100});
        } else {
            DrawCircle((int)centerX, (int)centerY, VALID_MOVE_RADIUS, Config::Colors::HINT_COLOR);
        }
    }
}
void Renderer::drawPromotionMenu(PieceColor color, const PromotionMenu& menu) const {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0, 0, 0, 150 }); 

    float boxSize = (float)menu.getBoxSize();
    float startX = (float)menu.getStartX(); 
    float startY = (float)menu.getStartY();
    const auto& options = menu.getOptions();

    float menuWidth = options.size() * boxSize;
    DrawRectangle((int)startX, (int)startY, (int)menuWidth, (int)boxSize, LIGHTGRAY);
    DrawRectangleLines((int)startX, (int)startY, (int)menuWidth, (int)boxSize, BLACK);

    Color pieceColor = (color == PieceColor::White) ? WHITE : Color{ 40, 40, 40, 255 };
    for (size_t i = 0; i < options.size(); ++i) {
        float xPos = startX + (i * boxSize) + (boxSize / 2.0f);
        float yPos = startY + (boxSize / 2.0f);
        d_drawers.at(options[i])->draw(xPos, yPos, pieceColor, boxSize);
    }
}
void Renderer::drawChrono(const char* bottomTime, const char* topTime, PieceColor currentTurn, int offsetX, int offsetY) {
    int width = 120;
    int height = 60;
    int fontSize = 30;
    int posX = offsetX + (8 * d_cellSize) + 20;

    int topY = offsetY; 
    int bottomY = offsetY + (8 * d_cellSize) - height;

    if (currentTurn == PieceColor::White) {
        DrawRectangle(posX, topY, width, height, BLACK);
        DrawRectangleLinesEx({(float)posX, (float)topY, (float)width, (float)height}, 1, GRAY);
        DrawText(topTime, posX + 20, topY + 15, fontSize, WHITE);

        DrawRectangle(posX, bottomY, width, height, RAYWHITE);
        DrawRectangleLinesEx({(float)posX, (float)bottomY, (float)width, (float)height}, 3, GOLD);
        DrawText(bottomTime, posX + 20, bottomY + 15, fontSize, BLACK);
    } else {
        DrawRectangle(posX, topY, width, height, RAYWHITE);
        DrawRectangleLinesEx({(float)posX, (float)topY, (float)width, (float)height}, 1, DARKGRAY);
        DrawText(topTime, posX + 20, topY + 15, fontSize, BLACK);

        DrawRectangle(posX, bottomY, width, height, BLACK);
        DrawRectangleLinesEx({(float)posX, (float)bottomY, (float)width, (float)height}, 3, GOLD);
        DrawText(bottomTime, posX + 20, bottomY + 15, fontSize, WHITE);
    }
}


void Renderer::drawEffects(const std::vector<VisualEffect>& effects) {
    for (const auto& effect : effects) {
        float drawX = d_offsetX + (effect.pos.x * d_cellSize);
        float drawY = d_offsetY + (effect.pos.y * d_cellSize);

        if (effect.type == EffectType::MeteoriteWarning) {
            DrawRectangle((int)drawX, (int)drawY, (int)d_cellSize, (int)d_cellSize, { 255, 0, 0, 150 });
            DrawText("!", (int)(drawX + d_cellSize/2.0f - 10), (int)(drawY + d_cellSize/2.0f - 20), 40, GREEN);
        }
    }
}
void Renderer::updateLayout(float cellSize, float offsetX, float offsetY) {
    d_cellSize = cellSize;
    d_offsetX = offsetX;
    d_offsetY = offsetY;
}

void Renderer::drawHands(const Player& player) const {
        d_cardRenderer.drawHands(player, GetMouseX(), GetMouseY());
}

int Renderer::getClickedCardIndex(int numCards, int mouseX, int mouseY) const {
    return d_cardRenderer.getClickedCardIndex(0, numCards, mouseX, mouseY);
}
void Renderer::drawPortals(const Board& board) {
    for (int x = 0; x < board.getWidth(); ++x) {
        for (int y = 0; y < board.getHeight(); ++y) {
            Position pos = {x, y};
            
            if (board.isPortal(pos)) {
                float centerX = d_offsetX + (x * d_cellSize) + (d_cellSize / 2.0f);
                float centerY = d_offsetY + (y * d_cellSize) + (d_cellSize / 2.0f);
                
                PortalDrawer::draw(centerX, centerY, d_cellSize);
            }
        }
    }
}

void Renderer::drawFogLayer(const Board& board, Position selectedTile, const std::vector<Position>& currentValidMoves, PieceColor currentColor) {
    for (int x = 0; x < board.getWidth(); ++x) {
        for (int y = 0; y < board.getHeight(); ++y) {
            Position pos = {x, y};
            const Tile& tile = board.getTile(pos);
            
            if (tile.isFoggy() && tile.getType() != TileType::Lava) {
                bool isRevealed = false;
                if (tile.hasPiece() && tile.getPiece()->getColor() == currentColor) {
                    isRevealed = true;
                }
                if (pos.x == selectedTile.x && pos.y == selectedTile.y) {
                    isRevealed = true;
                }
                for (const Position& move : currentValidMoves) {
                    if (pos.x == move.x && pos.y == move.y) {
                        isRevealed = true;
                        break;
                    }
                }
                if (!isRevealed) {
                    int displayY = (currentColor == PieceColor::Black) ? getFlippedY(y, board.getHeight()) : y;
                    float cornerX = d_offsetX + (x * d_cellSize);
                    float cornerY = d_offsetY + (displayY * d_cellSize);
                    DrawRectangle((int)cornerX, (int)cornerY, (int)d_cellSize, (int)d_cellSize, Config::Colors::FOG_COLOR);
                    DrawText("?", cornerX + d_cellSize/2 - 10, cornerY + d_cellSize/2 - 15, 30, DARKGRAY);
                }
            }
        }
    }
}
void Renderer::drawScore(int whiteScore, int blackScore) {
}
void Renderer::drawShop(const Shop& shop, const ShopMenu& menu, bool isShopOpen) {
    if (isShopOpen) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0, 0, 0, 180 });
        Rectangle panel = { (float)menu.getPanelX(), (float)menu.getPanelY(), (float)menu.getPanelWidth(), (float)menu.getPanelHeight() };
        DrawRectangleRec(panel, RAYWHITE);
        DrawRectangleLinesEx(panel, 4, GOLD);
        
        int titleWidth = MeasureText("MARCHE DU CHAOS", 30);
        DrawText("MARCHE DU CHAOS", menu.getPanelX() + (menu.getPanelWidth() - titleWidth) / 2, menu.getPanelY() + 20, 30, BLACK);

        const auto& cards = shop.getCards();
        for (size_t i = 0; i < cards.size(); ++i) {
            int cardX = menu.getStartX() + i * (menu.getCardWidth() + menu.getSpacing());
            int cardY = menu.getStartY();
            d_cardRenderer.drawCard(*cards[i], cardX, cardY, false);
            
            int price = cards[i]->getCost();
            const char* priceText = TextFormat("%d", price);
            int priceWidth = MeasureText(priceText, 24);
            int priceCenterX = cardX + (menu.getCardWidth() - priceWidth) / 2;
            int priceY = cardY + menu.getCardHeight() + 8;
            
            DrawText(priceText, priceCenterX, priceY, 24, GOLD);
        }
    }
}
