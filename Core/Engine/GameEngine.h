#pragma once
#include "../Core/Base/Board.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/UI/PromotionMenu.h"
#include "raylib.h"
#include "../Core/Audio/AudioManager.h"
#include "../Core/Time/Chono.h"
#include "../Core/Event/EventManager.h"

enum class GameState {
    Playing,
    WhiteWins,
    BlackWins,
    Stalemate 
};

class GameEngine {
public:
    GameEngine(int cellSize = Renderer::CELL_SIZE);
    void run();
    

private:

    GameState d_gameState = GameState::Playing;
    Board d_board;
    Renderer d_renderer;
    bool d_isPromoting = false;
    Position d_promotionPos = {-1, -1};

    PieceColor d_currentTurn;
    Position d_selectedTile;
    std::vector<Position> d_currentValidMoves;

    int d_cellSize;
    int d_margin;

    PromotionMenu d_promotionMenu;
    std::vector<PieceType> d_promotionOptions;
        inline static const std::vector<PieceType> BASE_PROMOTIONS = {
        PieceType::Queen, 
        PieceType::Rook, 
        PieceType::Knight, 
        PieceType::Bishop
    };

    AudioManager d_audioManager;
    EventManager d_eventManager;

    Chrono d_whiteClock; 
    Chrono d_blackClock;

    void initBoard();
    void handleInput(Position clickedPos);
    void handleCastling(Position startPos, Position targetPos);

    void handleEnPassant(Position startPos, Position targetPos);
    void updateEnPassantTarget(Position startPos, Position targetPos);

    bool checkPromotion(Position targetPos);
    void handlePromotion(int mouseX, int mouseY);

    std::vector<Position> filterLegalMoves(Position startPos, const std::vector<Position>& pseudoMoves);
    void updateGameState();

};