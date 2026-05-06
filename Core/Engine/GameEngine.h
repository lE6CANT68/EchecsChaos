#pragma once
#include "../Core/Base/Board.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/UI/PromotionMenu.h"
#include "raylib.h"
#include "../Core/Audio/AudioManager.h"
#include "../Core/Time/Chrono.h"
#include "../Core/Event/EventManager.h"
#include "../Base/Player.h"
#include "../Core/Pieces/Pawn.h"
#include "../Core/Pieces/Rook.h"
#include "../Core/Pieces/Knight.h"
#include "../Core/Pieces/Bishop.h"
#include "../Core/Pieces/Queen.h"
#include "../Core/Pieces/King.h"

#include "../Core/CardsList/MeteoriteCard.h"
#include "../Core/CardsList/TimeCard.h"
#include "../Core/CardsList/BlockCard.h"
#include "../Core/CardsList/PawnSprintCard.h"
#include "../Core/CardsList/FreezeCard.h"
#include "../Core/CardsList/FogCard.h"

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

   // PieceColor d_currentTurn;
    Position d_selectedTile;
    std::vector<Position> d_currentValidMoves;

    int d_cellSize;
    int d_offsetX;
    int d_offsetY;
    bool d_isTargeting = false;
    int d_pendingCardIndex = -1;

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

    std::vector<Player> d_players;
    int d_currentPlayerIndex;

    void initBoard();
    void handleInput(Position clickedPos);
    void handleCastling(Position startPos, Position targetPos);

    void handleEnPassant(Position startPos, Position targetPos);
    void updateEnPassantTarget(Position startPos, Position targetPos);

    bool checkPromotion(Position targetPos);
    void handlePromotion(int mouseX, int mouseY);

    std::vector<Position> filterLegalMoves(Position startPos, const std::vector<Position>& pseudoMoves);
    void updateGameState();

    void fillPlayerHand(Player& player);
    std::unique_ptr<Card> generateRandomCard();



};