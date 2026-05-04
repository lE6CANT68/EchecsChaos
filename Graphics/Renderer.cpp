#include "Renderer.h"

Renderer::Renderer(float cellSize) : d_cellSize(cellSize), d_margin(DEFAULT_MARGIN) {
    d_drawers[PieceType::Pawn] = std::make_unique<PawnDrawer>();
    d_drawers[PieceType::Bishop] = std::make_unique<BishopDrawer>();
    d_drawers[PieceType::Knight] = std::make_unique<KnightDrawer>();
    d_drawers[PieceType::Rook] = std::make_unique<RookDrawer>();
    d_drawers[PieceType::King] = std::make_unique<KingDrawer>();
    d_drawers[PieceType::Queen] = std::make_unique<QueenDrawer>();
}

void Renderer::draw(const Board& board, Position selectedPos, const std::vector<Position>& validMoves, Position kingInCheckPos) {
    drawBoard(board);
    drawTileHighlights(selectedPos, kingInCheckPos); 
    drawPieces(board);
    drawMoveHints(board, validMoves);                
}


void Renderer::drawBoard(const Board& board) {
    for (int x = 0; x < board.getWidth(); ++x) {
        for (int y = 0; y < board.getHeight(); ++y) {
            Color color = ((x + y) % 2 == 0) ? LIGHTGRAY : GRAY;
            
            float posX = d_margin + (x * d_cellSize);
            float posY = d_margin + (y * d_cellSize);

            DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, color);
        }
    }
}

void Renderer::drawPieces(const Board& board) {
    for (int x = 0; x < board.getWidth(); ++x) {
        for (int y = 0; y < board.getHeight(); ++y) {
            Position pos = {x, y};
            const Tile& tile = board.getTile(pos);

            if (tile.hasPiece()) {
                Piece* currentPiece = tile.getPiece();
                float centerX = d_margin + (x * d_cellSize) + (d_cellSize / 2.0f);
                float centerY = d_margin + (y * d_cellSize) + (d_cellSize / 2.0f);
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
        float posX = d_margin + (selectedPos.x * d_cellSize);
        float posY = d_margin + (selectedPos.y * d_cellSize);
        DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, HIGHLIGHT_COLOR); 
    }
    if (kingInCheckPos.x != -1 && kingInCheckPos.y != -1) {
        float posX = d_margin + (kingInCheckPos.x * d_cellSize);
        float posY = d_margin + (kingInCheckPos.y * d_cellSize);
        DrawRectangle((int)posX, (int)posY, (int)d_cellSize, (int)d_cellSize, { 220, 20, 20, 180 }); 
    }
}
void Renderer::drawMoveHints(const Board& board, const std::vector<Position>& validMoves) {
    for (const Position& move : validMoves) {
        float centerX = d_margin + (move.x * d_cellSize) + (d_cellSize / 2.0f);
        float centerY = d_margin + (move.y * d_cellSize) + (d_cellSize / 2.0f);
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
void Renderer::drawChrono(const char* whiteTime, const char* blackTime, PieceColor currentTurn) {
    int posX = 690; 
    int width = 120;
    int height = 60;
    int fontSize = 30;

    DrawRectangle(posX, 50, width, height, BLACK);
    if (currentTurn == PieceColor::Black) {
        DrawRectangleLinesEx({(float)posX, 50, (float)width, (float)height}, 3, GOLD);
    } else {
        DrawRectangleLinesEx({(float)posX, 50, (float)width, (float)height}, 1, GRAY);
    }
    DrawText(blackTime, posX + 20, 65, fontSize, WHITE);

    DrawRectangle(posX, 690, width, height, RAYWHITE);
    if (currentTurn == PieceColor::White) {
        DrawRectangleLinesEx({(float)posX, 690, (float)width, (float)height}, 3, GOLD);
    } else {
        DrawRectangleLinesEx({(float)posX, 690, (float)width, (float)height}, 1, DARKGRAY);
    }
    
    DrawText(whiteTime, posX + 20, 705, fontSize, BLACK);
}

void Renderer::drawEffects(const std::vector<VisualEffect>& effects) {
    for (const auto& effect : effects) {
        float drawX = d_margin + (effect.pos.x * d_cellSize);
        float drawY = d_margin + (effect.pos.y * d_cellSize);

        if (effect.type == EffectType::MeteoriteWarning) {
            DrawRectangle((int)drawX, (int)drawY, (int)d_cellSize, (int)d_cellSize, { 255, 0, 0, 150 });
            DrawText("!", (int)(drawX + d_cellSize/2.0f - 10), (int)(drawY + d_cellSize/2.0f - 20), 40, GREEN);
        }
    }
}