#include "GameEngine.h"



GameEngine::GameEngine(int cellSize) 
    : d_board(8, 8), 
      d_renderer(cellSize), 
      d_selectedTile(Position::NONE),
      d_cellSize(cellSize),
      d_offsetX(Config::Graphics::DEFAULT_OFFSETX),  
      d_offsetY(Config::Graphics::DEFAULT_OFFSETY),
      d_promotionMenu({BASE_PROMOTIONS}),

      d_eventManager{d_audioManager}
{
    d_players.push_back(Player(PieceColor::White, Chrono::STANDARD_TIME, Chrono::STANDARD_INCREMENT));
    d_players.push_back(Player(PieceColor::Black, Chrono::STANDARD_TIME, Chrono::STANDARD_INCREMENT));
    d_currentPlayerIndex = 0;

    fillPlayerHand(d_players[0]);
    fillPlayerHand(d_players[1]);
    
    initBoard();


}

void GameEngine::initBoard() {
    const int bBack = Config::Board::BLACK_BACK_ROW;
    const int bPawn = Config::Board::BLACK_PAWN_ROW;
    const int wPawn = Config::Board::WHITE_PAWN_ROW;
    const int wBack = Config::Board::WHITE_BACK_ROW;
    for (int i = 0; i < Config::Board::WIDTH; ++i) {
        d_board.getTile({i, bPawn}).setPiece(std::make_unique<Pawn>(PieceColor::Black));
        d_board.getTile({i, wPawn}).setPiece(std::make_unique<Pawn>(PieceColor::White));
    }

    // Pièces Noires
    d_board.getTile({0, bBack}).setPiece(std::make_unique<Rook>(PieceColor::Black));
    d_board.getTile({1, bBack}).setPiece(std::make_unique<Knight>(PieceColor::Black));
    d_board.getTile({2, bBack}).setPiece(std::make_unique<Bishop>(PieceColor::Black));
    d_board.getTile({3, bBack}).setPiece(std::make_unique<Queen>(PieceColor::Black));
    d_board.getTile({4, bBack}).setPiece(std::make_unique<King>(PieceColor::Black));
    d_board.getTile({5, bBack}).setPiece(std::make_unique<Bishop>(PieceColor::Black));
    d_board.getTile({6, bBack}).setPiece(std::make_unique<Knight>(PieceColor::Black));
    d_board.getTile({7, bBack}).setPiece(std::make_unique<Rook>(PieceColor::Black));

    // Pièces Blanches
    d_board.getTile({0, wBack}).setPiece(std::make_unique<Rook>(PieceColor::White));
    d_board.getTile({1, wBack}).setPiece(std::make_unique<Knight>(PieceColor::White));
    d_board.getTile({2, wBack}).setPiece(std::make_unique<Bishop>(PieceColor::White));
    d_board.getTile({3, wBack}).setPiece(std::make_unique<Queen>(PieceColor::White));
    d_board.getTile({4, wBack}).setPiece(std::make_unique<King>(PieceColor::White));
    d_board.getTile({5, wBack}).setPiece(std::make_unique<Bishop>(PieceColor::White));
    d_board.getTile({6, wBack}).setPiece(std::make_unique<Knight>(PieceColor::White));
    d_board.getTile({7, wBack}).setPiece(std::make_unique<Rook>(PieceColor::White));
}

void GameEngine::handleInput(Position clickedPos) {
    if (!d_board.isinBounds(clickedPos)) return;
    if (d_board.getTile(clickedPos).getType() == TileType::Frozen) {
        d_selectedTile = Position::NONE;
        d_currentValidMoves.clear();
        return; 
    }

    PieceColor currentColor = d_players[d_currentPlayerIndex].getColor();

    bool moveExecuted = BoardInteractionManager::tryExecuteMove(d_board, clickedPos, d_selectedTile, d_currentValidMoves, d_audioManager,d_players[d_currentPlayerIndex]);

    if (moveExecuted) {
        d_currentValidMoves.clear();

        if (PromotionHandler::checkPromotion(d_board, clickedPos)) {
            d_isPromoting = true;
            d_promotionPos = clickedPos;
        } else {
            d_players[d_currentPlayerIndex].getClock().addIncrement();
            d_players[d_currentPlayerIndex].resetTurn(); 
            d_currentPlayerIndex = (d_currentPlayerIndex + 1) % d_players.size();
            updateGameState();

            if (d_gameState == GameState::Playing) {
                d_eventManager.processActiveEvents(d_board);
                d_eventManager.triggerRandomEvent(d_board);
            }
        }
    } else {
        BoardInteractionManager::updateSelection(d_board, clickedPos, currentColor, d_selectedTile, d_currentValidMoves);
    }
}



void GameEngine::updateGameState() {
    bool hasAnyLegalMove = false;
    PieceColor currentColor = d_players[d_currentPlayerIndex].getColor();

    for (int x = 0; x < d_board.getWidth(); ++x) {
        for (int y = 0; y < d_board.getHeight(); ++y) {
            
            const Tile& tile = d_board.getTile({x, y});

            if (tile.hasPiece() && tile.getPiece()->getColor() == currentColor) {

                std::vector<Position> pseudoMoves = tile.getPiece()->getValidMoves({x, y}, d_board);
                std::vector<Position> legalMoves = MoveValidator::filterLegalMoves(d_board,{x, y}, pseudoMoves);

                if (!legalMoves.empty()) {
                    hasAnyLegalMove = true;
                    break; 
                }
            }
        }
        if (hasAnyLegalMove) break;
    }

    if (!hasAnyLegalMove) {
        if (d_board.isKingInCheck(currentColor)) {
            d_gameState = (currentColor == PieceColor::White) ? GameState::BlackWins : GameState::WhiteWins;
            if(d_gameState == GameState::BlackWins){
                d_audioManager.playGameOver();
            }
            else{
                d_audioManager.playVictory();
            }
            
        } 
        else {
            d_gameState = GameState::Stalemate;
            d_audioManager.playStalemate();
        }
    }
}

void GameEngine::run() {
    while (!WindowShouldClose()) {
        
        updateSystems(); 
        
        if (d_gameState == GameState::Playing) {
            processInput(); 
        }
        
        renderFrame(); 
        
    }
}
std::unique_ptr<Card> GameEngine::generateRandomCard() {
    int roll = GetRandomValue(0, 8); 
     roll = 8;
    switch (roll) {
        case 0: return std::make_unique<MeteoriteCard>();
        case 1: return std::make_unique<TimeCard>();
        case 2: return std::make_unique<BlockCard>();
        case 3: return std::make_unique<PawnSprintCard>();
        case 4: return std::make_unique<FreezeCard>();
        case 5: return std::make_unique<FogCard>();
        case 6: return std::make_unique<HideTimeCard>();
        case 7: return std::make_unique<LavaWallCard>();
        case 8: return std::make_unique<TeleportCard>();
        default: return std::make_unique<MeteoriteCard>(); 
    }
}


void GameEngine::fillPlayerHand(Player& player) {
    while (player.getHand().size() < 3) {
        player.drawCard(generateRandomCard());
    }
}

const char* GameEngine::getPlayerTimeString(int playerIndex) {
    PieceColor color = d_players[playerIndex].getColor();
    bool isHacked = false;
    if (color == PieceColor::White) {
        isHacked = d_eventManager.hasGlobalEffect(GlobalEffect::HideTimeWhite) || 
                   d_eventManager.hasGlobalEffect(GlobalEffect::HideTimeBoth);
    } 
    else {
        isHacked = d_eventManager.hasGlobalEffect(GlobalEffect::HideTimeBlack) || 
                   d_eventManager.hasGlobalEffect(GlobalEffect::HideTimeBoth);
    }
    if (isHacked) {
        return d_players[playerIndex].getClock().getScrambledTime();
    } 
    else {
        return d_players[playerIndex].getClock().getFormattedTime();
    }
}
void GameEngine::updateSystems() {
    d_audioManager.updateMusic();
    d_eventManager.update();

    if (d_gameState == GameState::Playing && !d_isPromoting) { 
        Player& currentPlayer = d_players[d_currentPlayerIndex];
        currentPlayer.getClock().update();
        
        if (currentPlayer.getClock().isZero()) {
            PieceColor currentColor = currentPlayer.getColor();
            d_gameState = (currentColor == PieceColor::White) ? GameState::BlackWins : GameState::WhiteWins; 
            d_audioManager.playGameOver();
        }
    }
}
void GameEngine::processInput() {
    Player& currentPlayer = d_players[d_currentPlayerIndex];

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        bool actionExecuted = false;
        if (d_isTargeting) {
            int boardX = (mouseX - d_offsetX) / d_cellSize;
            int boardY = (mouseY - d_offsetY) / d_cellSize;
            Position targetPos = {boardX, boardY};

            if (d_board.isinBounds(targetPos)) {
                Card* pendingCard = currentPlayer.getHand()[d_pendingCardIndex].get();
                if (pendingCard->requiresTwoTargets()) {
                    if (!d_firstTarget.isValid()) {
                        if (pendingCard->isValidTarget(d_board, targetPos)) {
                            d_firstTarget = targetPos; 
                        }
                    } else {
                        if (pendingCard->isValidSecondTarget(d_board, d_firstTarget, targetPos)) {
                            pendingCard->playTwoTargets(currentPlayer, d_board, d_eventManager, d_firstTarget, targetPos);
                            fillPlayerHand(currentPlayer);
                            d_isTargeting = false;
                            d_pendingCardIndex = -1;
                            d_firstTarget = Position::NONE; 
                        }
                    }
                } 
                else {
                    if (pendingCard->isValidTarget(d_board, targetPos)) {
                        pendingCard->play(currentPlayer, d_board, d_eventManager, targetPos);
                        fillPlayerHand(currentPlayer);
                        d_isTargeting = false;
                        d_pendingCardIndex = -1;
                    }
                }
            }
            actionExecuted = true; 
        }
        else if (!currentPlayer.hasPlayedCardThisTurn()) {
            int clickedCard = d_renderer.getClickedCardIndex(d_currentPlayerIndex, currentPlayer.getHand().size(), mouseX, mouseY);

            if (clickedCard != -1) {
                if (currentPlayer.getHand()[clickedCard]->requiresTarget()) {
                    d_isTargeting = true;
                    d_pendingCardIndex = clickedCard;
                } else {
                    currentPlayer.playCard(clickedCard, d_board, d_eventManager);
                    fillPlayerHand(currentPlayer);
                }
                actionExecuted = true;
            }
        }
        if (!actionExecuted && !d_isTargeting) {
            if (d_isPromoting) {
                bool promotionSuccess = PromotionHandler::handlePromotion(d_board, d_promotionMenu, d_promotionPos, mouseX, mouseY);
                if (promotionSuccess) {
                    d_isPromoting = false;
                    d_promotionPos = Position::NONE;
                    currentPlayer.getClock().addIncrement();
                    currentPlayer.resetTurn();
                    d_currentPlayerIndex = (d_currentPlayerIndex + 1) % d_players.size();
                    updateGameState();
                }
            } else {
                int boardX = (mouseX - d_offsetX) / d_cellSize;
                int boardY = (mouseY - d_offsetY) / d_cellSize;
                handleInput({boardX, boardY}); 
            }
        }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && d_isTargeting) {
        d_isTargeting = false;
        d_pendingCardIndex = -1;
        d_firstTarget = Position::NONE;
    }
}
void GameEngine::renderFrame() {
    PieceColor currentColor = d_players[d_currentPlayerIndex].getColor();

    std::vector<Position> globalVision;
    for (int x = 0; x < d_board.getWidth(); ++x) {
        for (int y = 0; y < d_board.getHeight(); ++y) {
            Position p = {x, y};
            const Tile& tile = d_board.getTile(p);
            if (tile.hasPiece() && tile.getPiece()->getColor() == currentColor) {
                std::vector<Position> vision = tile.getPiece()->getValidMoves(p, d_board);
                globalVision.insert(globalVision.end(), vision.begin(), vision.end());
            }
        }
    }

    BeginDrawing();
        ClearBackground(DARKGRAY);
        
        Position checkedKingPos = Position::NONE;
        if (d_board.isKingInCheck(currentColor)) {
            checkedKingPos = d_board.getKingPosition(currentColor);
        }
        int whiteScore = d_players[0].getScore(); 
        int blackScore = d_players[1].getScore();

        d_renderer.draw(d_board, d_selectedTile, d_currentValidMoves, currentColor, whiteScore, blackScore, checkedKingPos);
        d_renderer.drawChrono(getPlayerTimeString(0), getPlayerTimeString(1), currentColor, d_offsetX, d_offsetY);

        std::vector<VisualEffect> chaosEffects = d_eventManager.getActiveVisualEffects();
        d_renderer.drawEffects(chaosEffects);

        if (d_eventManager.hasActiveMessage()) {
            int textWidth = MeasureText(d_eventManager.getActiveMessage(), 25);
            DrawRectangle(400 - (textWidth/2) - 10, 390, textWidth + 20, 40, {0, 0, 0, 180});
            DrawText(d_eventManager.getActiveMessage(), 400 - (textWidth/2), 395, 25, RED);
        }

        if (d_isPromoting) {
            PieceColor promoColor = d_board.getTile(d_promotionPos).getPiece()->getColor();
            d_renderer.drawPromotionMenu(promoColor, d_promotionMenu);
        }
        
        if (d_gameState != GameState::Playing) {
            DrawRectangle(0, 0, Config::Graphics::CONFIG_WINDOW_HEIGHT, Config::Graphics::CONFIG_WINDOW_WIDTH, { 0, 0, 0, 200 }); 
            const char* message = "";
            if (d_gameState == GameState::WhiteWins) message = "LES BLANCS GAGNENT !"; 
            if (d_gameState == GameState::BlackWins) message = "LES NOIRS GAGNENT !";  
            if (d_gameState == GameState::Stalemate) message = "PAT ! MATCH NUL !";

            int textWidth = MeasureText(message, 30);
            DrawText(message, 400 - (textWidth / 2), 380, 30, RAYWHITE);
        }
        
        d_renderer.drawHands(d_players);

        
        if (d_isTargeting) {
            DrawRectangle(0, 0, GetScreenWidth(), 40, Fade(BLACK, 0.8f));
            DrawText("MODE CIBLAGE : Cliquez sur une case du plateau (Clic droit pour annuler)", 150, 10, 20, YELLOW);
        }
        
    EndDrawing();
}

