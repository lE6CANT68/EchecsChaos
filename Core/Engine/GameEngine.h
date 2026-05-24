#pragma once
#include "../Core/Base/Board.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/UI/PromotionMenu.h"
#include <future>
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
#include "../Graphics/UI/Settings.h"
#include "../Graphics/UI/SettingsScreen.h"
#include "../Graphics/UI/TitleScreen.h"

#include "../Core/CardsList/MeteoriteCard.h"
#include "../Core/CardsList/TimeCard.h"
#include "../Core/CardsList/BlockCard.h"
#include "../Core/CardsList/PawnSprintCard.h"
#include "../Core/CardsList/FreezeCard.h"
#include "../Core/CardsList/FogCard.h"
#include "../Core/CardsList/HideTimeCard.h"
#include "../Core/CardsList/LavaWallCard.h"
#include "../Core/CardsList/TeleportCard.h"
#include "../Core/CardsList/MysteryCard.h"

#include "SpecialMoveHandler.h"
#include "MoveValidator.h"
#include "PromotionHandler.h"
#include "BoardInteractionManager.h"
#include "../Core/Engine/Shop.h"

#include "../Graphics/UI/SettingsScreen.h"

enum class GameState {
    TitleScreen,
    Settings,
    Playing,
    WAITING_FOR_AI,
    WhiteWins,
    BlackWins,
    Stalemate,
    WAITING_FOR_ANALYSIS,
    SHOW_ANALYSIS
};

class GameEngine {
public:
    GameEngine(int cellSize = Config::Graphics::CELL_SIZE);
    void run();
    

private:

    GameState d_gameState = GameState::TitleScreen;
    bool d_shouldQuit = false;
    Settings d_settings;
    SettingsScreen d_settingsScreen;
    std::future<std::string> d_aiResponseFuture;
    TitleScreen d_titleScreen;
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
    int d_turnCount = 0; // Compte les tours pour savoir quand faire jouer le canard

    void initBoard();
    void handleInput(Position clickedPos);
    int getFlippedBoardY(int boardY) const;

    void updateGameState();
    void updateSystems();
    void processInput();
    void renderFrame();
    void updateBoardLayout();

    void fillPlayerHand(Player& player);
    std::unique_ptr<Card> generateRandomCard();

    const char* getPlayerTimeString(int playerIndex);

    void handleCastling(Position startPos, Position targetPos);
    void handleEnPassant(Position startPos, Position targetPos);
    void updateEnPassantTarget(Position startPos, Position targetPos);
    void applySlipperyTerrain(Position startPos, Position targetPos);
    void endPlayerTurn();
    std::vector<Position> filterLegalMoves(Position startPos, const std::vector<Position>& pseudoMoves);

    Position d_firstTarget = Position::NONE;
    Shop d_shop;
    ShopMenu d_shopMenu;
    bool d_isShopOpen = false;
    Button d_shopButton{ 10, 10, "BOUTIQUE" };
    bool d_isHandVisible = true;
    Button d_toggleHandButton{ 150, 10, "CARTES (ON/OFF)" };
    Button d_aiButton{ 380, 10, "IA" };
    bool d_isAIPromptOpen = false;
    std::string d_aiPromptText = "";

    std::vector<std::string> d_boardHistory;
    std::future<std::string> d_aiAnalysisFuture;
    Button d_analyzeButton{ 300, 500, "ANALYSER LA PARTIE" };
    
    struct AnalysisMove {
        int turn;
        std::string fen;
        std::string comment;
    };
    std::vector<AnalysisMove> d_analysisMoves;
    int d_currentAnalysisIndex = 0;
    Button d_nextAnalysisButton{ 600, 700, "SUIVANT" };
    Button d_prevAnalysisButton{ 100, 700, "PRECEDENT" };
    Button d_closeAnalysisButton{ 350, 700, "FERMER" };
    Board d_analysisBoard;



};