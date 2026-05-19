#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(float cellSize) : d_cellSize{cellSize}, d_offsetX{DEFAULT_OFFSETX} ,d_offsetY{DEFAULT_OFFSETY} {
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
    
    // Initialiser les dimensions une seule fois (dès que Raylib est prêt)
    if (!d_initializedDimensions && screenWidth > 0 && screenHeight > 0) {
        d_initializedDimensions = true;
        d_lastBoardWidth = currentWidth;
        
        int availableHeight = screenHeight - 250; // Moins d'espace réservé
        int availableWidth = screenWidth - 80;
        
        float maxCellSize = std::min(
            availableWidth / (float)currentWidth,
            availableHeight / (float)currentHeight
        );
        
        d_cellSize = maxCellSize;
        d_offsetX = (screenWidth - (currentWidth * d_cellSize)) / 2.0f;
        d_offsetY = (screenHeight - (currentHeight * d_cellSize)) / 2.0f; // Centrage parfait
    }
    
    // Si la taille du board a changé, recalculer les dimensions
    if (currentWidth != d_lastBoardWidth && screenWidth > 0 && screenHeight > 0) {
        d_lastBoardWidth = currentWidth;
        
        // Réserver de l'espace pour les cartes
        int availableHeight = screenHeight - 250; // Espace pour cartes haut/bas
        int availableWidth = screenWidth - 80;    // marges latérales
        
        // Calculer la taille maximale d'une cellule pour remplir l'espace
        float maxCellSize = std::min(
            availableWidth / (float)currentWidth,
            availableHeight / (float)currentHeight
        );
        
        // Définir la taille de cellule et les offsets pour centrer le plateau
        d_cellSize = maxCellSize;
        d_offsetX = (screenWidth - (currentWidth * d_cellSize)) / 2.0f;
        d_offsetY = (screenHeight - (currentHeight * d_cellSize)) / 2.0f; // Centrage parfait
        
        // Redimensionner les cartes aussi
        if (currentWidth == 10) {
            d_cardRenderer.updateCardDimensions(10);
        } else {
            d_cardRenderer.updateCardDimensions(8);
        }
    }
}

void Renderer::draw(const Board& board, Position selectedPos, const std::vector<Position>& validMoves,PieceColor currentColor,Position kingInCheckPos) {
    updateDimensionsForBoard(board);
    drawBoard(board);
    drawBoardBorder(board);
    drawPortals(board);
    drawTileHighlights(selectedPos, kingInCheckPos); 
    drawPieces(board);
    drawMoveHints(board, validMoves);   
    drawFogLayer(board, selectedPos, validMoves,currentColor);
    
}
void Renderer::drawBoard(const Board& board) {
    for (int x = 0; x < board.getWidth(); ++x) {
        for (int y = 0; y < board.getHeight(); ++y) {
            Color color = ((x + y) % 2 == 0) ? LIGHTGRAY : GRAY;
            
            float posX = d_offsetX + (x * d_cellSize);
            float posY = d_offsetY + (y * d_cellSize);

            
            DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, color);

            
            drawTileDecoration(board.getTile({x, y}).getType(), posX, posY);
        }
    }
}

void Renderer::drawBoardBorder(const Board& board) {
    // Calculer les coordonnées de la bordure
    float startX = d_offsetX;
    float startY = d_offsetY;
    float endX = d_offsetX + (board.getWidth() * d_cellSize);
    float endY = d_offsetY + (board.getHeight() * d_cellSize);
    
    // Dessiner les 4 côtés de la bordure avec une ligne épaisse
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
            
            // DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, Fade(ORANGE, 0.6f));
            break;
            
        case TileType::Ice:
            
            // DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, Fade(SKYBLUE, 0.5f));
            break;
            
        case TileType::Normal:
        default:
            // On ne dessine rien par-dessus
            break;
        case TileType::Frozen:
            DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, Fade(SKYBLUE, 0.5f));
    
    
             DrawRectangleLinesEx({posX + 2, posY + 2, d_cellSize - 4, d_cellSize - 4}, 3, Fade(WHITE, 0.8f));
            break;
    }
}

void Renderer::drawPieces(const Board& board) {
    for (int x = 0; x < board.getWidth(); ++x) {
        for (int y = 0; y < board.getHeight(); ++y) {
            Position pos = {x, y};
            const Tile& tile = board.getTile(pos);

            if (tile.hasPiece()) {
                Piece* currentPiece = tile.getPiece();
                float centerX = d_offsetX + (x * d_cellSize) + (d_cellSize / 2.0f);
                float centerY = d_offsetY + (y * d_cellSize) + (d_cellSize / 2.0f);
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
void Renderer::drawTileHighlights(Position selectedPos, Position kingInCheckPos) {
    if (selectedPos.x != -1 && selectedPos.y != -1) {
        float posX = d_offsetX + (selectedPos.x * d_cellSize);
        float posY = d_offsetY + (selectedPos.y * d_cellSize);
        DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, HIGHLIGHT_COLOR); 
    }
    if (kingInCheckPos.x != -1 && kingInCheckPos.y != -1) {
        float posX = d_offsetX + (kingInCheckPos.x * d_cellSize);
        float posY = d_offsetY + (kingInCheckPos.y * d_cellSize);
        DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, { 220, 20, 20, 180 }); 
    }
}
void Renderer::drawMoveHints(const Board& board, const std::vector<Position>& validMoves) {
    for (const Position& move : validMoves) {
        float centerX = d_offsetX + (move.x * d_cellSize) + (d_cellSize / 2.0f);
        float centerY = d_offsetY + (move.y * d_cellSize) + (d_cellSize / 2.0f);
        if (board.getTile(move).hasPiece() || move == board.getEnPassantTarget()) {
            DrawRing({centerX, centerY}, d_cellSize/2.0f - 8.0f, d_cellSize/2.0f - 2.0f, 0, 360, 32, {0, 0, 0, 100});
        } else {
            DrawCircle((int)centerX, (int)centerY, VALID_MOVE_RADIUS, HINT_COLOR);
        }
    }
}
void Renderer::drawPromotionMenu(PieceColor color, const PromotionMenu& menu) const {
    DrawRectangle(0, 0, 800, 800, { 0, 0, 0, 150 }); 

    float boxSize = (float)menu.getBoxSize();
    float startX = (float)menu.getStartX();
    float startY = (float)menu.getStartY();
    const auto& options = menu.getOptions();

    DrawRectangle((int)startX, (int)startY, (int)(boxSize * options.size()), (int)boxSize, LIGHTGRAY);
    DrawRectangleLines((int)startX, (int)startY, (int)(boxSize * options.size()), (int)boxSize, BLACK);

    Color pieceColor = (color == PieceColor::White) ? WHITE : Color{ 40, 40, 40, 255 };

    for (size_t i = 0; i < options.size(); ++i) {
        float xPos = startX + (i * boxSize) + (boxSize / 2.0f);
        float yPos = startY + (boxSize / 2.0f);
        d_drawers.at(options[i])->draw(xPos, yPos, pieceColor, boxSize);
    }
}
void Renderer::drawChrono(const char* whiteTime, const char* blackTime, PieceColor currentTurn, int offsetX, int offsetY) {
    int width = 120;
    int height = 60;
    int fontSize = 30;
    int posX = offsetX + (8 * d_cellSize) + 20;

  
    int blackY = offsetY; 

    DrawRectangle(posX, blackY, width, height, BLACK);
    if (currentTurn == PieceColor::Black) {
        DrawRectangleLinesEx({(float)posX, (float)blackY, (float)width, (float)height}, 3, GOLD);
    } else {
        DrawRectangleLinesEx({(float)posX, (float)blackY, (float)width, (float)height}, 1, GRAY);
    }
    // On centre le texte par rapport à blackY
    DrawText(blackTime, posX + 20, blackY + 15, fontSize, WHITE);


   
    int whiteY = offsetY + (8 * d_cellSize) - height;

    DrawRectangle(posX, whiteY, width, height, RAYWHITE);
    if (currentTurn == PieceColor::White) {
        DrawRectangleLinesEx({(float)posX, (float)whiteY, (float)width, (float)height}, 3, GOLD);
    } else {
        DrawRectangleLinesEx({(float)posX, (float)whiteY, (float)width, (float)height}, 1, DARKGRAY);
    }
    
    DrawText(whiteTime, posX + 20, whiteY + 15, fontSize, BLACK);
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
void Renderer::drawHands(const std::vector<Player>& players) const {
        d_cardRenderer.drawHands(players, GetMouseX(), GetMouseY());
}

int Renderer::getClickedCardIndex(int playerIndex, int numCards, int mouseX, int mouseY) const {
    return d_cardRenderer.getClickedCardIndex(playerIndex, numCards, mouseX, mouseY);
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
            
            if (tile.isFoggy()) {
                bool isRevealed = false;

                // ---> LA NOUVEAUTÉ EST ICI <---
                // 1. On voit toujours NOS propres pièces !
                if (tile.hasPiece() && tile.getPiece()->getColor() == currentColor) {
                    isRevealed = true;
                }

                // 2. Révélé si c'est la case cliquée
                if (pos.x == selectedTile.x && pos.y == selectedTile.y) {
                    isRevealed = true;
                }

                // 3. Révélé si c'est un mouvement possible
                for (const Position& move : currentValidMoves) {
                    if (pos.x == move.x && pos.y == move.y) {
                        isRevealed = true;
                        break;
                    }
                }

                // On dessine le brouillard seulement si ce n'est pas révélé
                if (!isRevealed) {
                    float cornerX = d_offsetX + (x * d_cellSize);
                    float cornerY = d_offsetY + (y * d_cellSize);
                    DrawRectangle((int)cornerX, (int)cornerY, (int)d_cellSize, (int)d_cellSize, {15, 15, 15, 255});
                    DrawText("?", cornerX + d_cellSize/2 - 10, cornerY + d_cellSize/2 - 15, 30, DARKGRAY);
                }
            }
        }
    }
}
