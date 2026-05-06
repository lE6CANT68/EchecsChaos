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
#include "../Core/CardsList/HideTimeCard.h"

#include "SpecialMoveHandler.h"
#include "MoveValidator.h"
#include "PromotionHandler.h"
#include "BoardInteractionManager.h"

enum class GameState {
    Playing,
    WhiteWins,
    BlackWins,
    Stalemate 
};

class GameEngine {
public:
    GameEngine(int cellSize = Config::Graphics::CELL_SIZE);
    void run();
    

private:

    GameState d_gameState = GameState::Playing;
    Board d_board;
    Renderer d_renderer;
    bool d_isPromoting = false;
    Position d_promotionPos = Position::NONE;

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


    void updateGameState();
    void updateSystems();
    void processInput();
    void renderFrame();

    void fillPlayerHand(Player& player);
    std::unique_ptr<Card> generateRandomCard();

    const char* getPlayerTimeString(int playerIndex);



};