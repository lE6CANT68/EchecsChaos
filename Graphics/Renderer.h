#pragma once
#include "raylib.h"
#include "../Core/Base/Board.h"
#include "PieceDrawer.h"
#include "../Core/Base/Piece.h"
#include <map>
#include <memory>

#include "Pieces/PawnDrawer.h"
#include "Pieces/BishopDrawer.h"
#include "Pieces/KnightDrawer.h"
#include "Pieces/RookDrawer.h"
#include "Pieces/KingDrawer.h"
#include "Pieces/QueenDrawer.h"
#include "../Graphics/UI/PromotionMenu.h"
#include "../Graphics/VisualEffect.h"

#include "CardRenderer.h"




class Renderer {
public:
    Renderer(float cellSize = CELL_SIZE);
    static constexpr float CELL_SIZE = 80.0f;
    static constexpr int DEFAULT_OFFSETX = 200.0f;
    static constexpr int DEFAULT_OFFSETY = 200.0f;


    void draw(const Board& board, Position selectedPos, const std::vector<Position>& validMoves,PieceColor currentColor,Position kingInCheckPos = {-1, -1});
    void drawPromotionMenu(PieceColor color,const PromotionMenu& menu) const;
    void drawChrono(const char* whiteTime, const char* blackTime, PieceColor currentTurn, int offsetX, int offsetY);
    void drawEffects(const std::vector<VisualEffect>& effects);
    void drawHands(const std::vector<Player>& players) const ;
    

    int getClickedCardIndex(int playerIndex, int numCards, int mouseX, int mouseY) const;



private:
    float d_cellSize;
    int d_offsetX;
    int d_offsetY;

    std::map<PieceType, std::unique_ptr<PieceDrawer>> d_drawers;


    void drawBoard(const Board& board);
    void drawPieces(const Board& board);
    void drawTileHighlights(Position selectedPos, Position kingInCheckPos);
    void drawMoveHints(const Board& board, const std::vector<Position>& validMoves);
    void drawTileDecoration(TileType type, float posX, float posY) const;
    void drawFogLayer(const Board& board, Position selectedTile, const std::vector<Position>& currentValidMoves, PieceColor currentColor);
    
    
    
    

    
   

    static constexpr float FALLBACK_PIECE_RADIUS = 20.0f;
    static constexpr float VALID_MOVE_RADIUS = 15.0f;
    static constexpr Color HIGHLIGHT_COLOR = { 46, 204, 113, 150 }; 
    static constexpr Color HINT_COLOR = { 0, 0, 0, 70 };

    CardRenderer d_cardRenderer;
};