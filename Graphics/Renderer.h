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
#include "Pieces/IdiotDrawer.h"
#include "Pieces/PionDebileDrawer.h"
#include "Pieces/DuckDrawer.h"
#include "PortalDrawer.h"
#include "../Graphics/UI/PromotionMenu.h"
#include "../Graphics/VisualEffect.h"

#include "CardRenderer.h"

#include "../Core/Config/Constant.h"

#include "../Core/Engine/Shop.h"
#include "../Graphics/UI/ShopMenu.h"




class Renderer {
public:
    Renderer(float cellSize);

    void updateLayout(float cellSize, float offsetX, float offsetY);
    void draw(const Board& board, Position selectedPos, const std::vector<Position>& validMoves,PieceColor currentColor,int whiteScore, int blackScore,Position kingInCheckPos = Position::NONE);
    void drawPromotionMenu(PieceColor color,const PromotionMenu& menu) const;
    void drawChrono(const char* whiteTime, const char* blackTime, PieceColor currentTurn, int offsetX, int offsetY);
    void drawEffects(const std::vector<VisualEffect>& effects);
    void drawHands(const std::vector<Player>& players) const ;
    void updateDimensionsForBoard(const Board& board);

    int getClickedCardIndex(int playerIndex, int numCards, int mouseX, int mouseY) const;
    
    // Accesseurs pour les dimensions
    float getCellSize() const { return d_cellSize; }
    int getOffsetX() const { return d_offsetX; }
    int getOffsetY() const { return d_offsetY; }
    void drawHands(const Player& player) const ;
    void drawScore(int whiteScore, int blackScore);
    void drawShop(const Shop& shop, const ShopMenu& menu, bool isShopOpen);
    

    int getClickedCardIndex(int numCards, int mouseX, int mouseY) const;
    Rectangle getShopPanelBounds() const { return { 150, 100, 700, 400 }; } // Le menu central



private:
    float d_cellSize;
    int d_offsetX;
    int d_offsetY;
    int d_lastBoardWidth = 8; // Pour détecter les changements de taille
    bool d_initializedDimensions = false; // Initialisation faite une fois

    std::map<PieceType, std::unique_ptr<PieceDrawer>> d_drawers;

    int getFlippedY(int y, int boardHeight) const { return boardHeight - 1 - y; }

    void drawBoard(const Board& board);
    void drawBoardBorder(const Board& board);
    void drawPieces(const Board& board);
    void drawTileHighlights(Position selectedPos, Position kingInCheckPos);
    void drawMoveHints(const Board& board, const std::vector<Position>& validMoves);
    void drawBoard(const Board& board, PieceColor currentColor);
    void drawPieces(const Board& board, PieceColor currentColor);
    void drawTileHighlights(Position selectedPos, Position kingInCheckPos, const Board& board, PieceColor currentColor);
    void drawMoveHints(const Board& board, const std::vector<Position>& validMoves, PieceColor currentColor);
    void drawTileDecoration(TileType type, float posX, float posY) const;
    void drawPortals(const Board& board);
    void drawFogLayer(const Board& board, Position selectedTile, const std::vector<Position>& currentValidMoves, PieceColor currentColor);
    
    
    


    
   

    static constexpr float FALLBACK_PIECE_RADIUS = 20.0f;
    static constexpr float VALID_MOVE_RADIUS = 15.0f;


    CardRenderer d_cardRenderer;
};