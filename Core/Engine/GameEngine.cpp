#include "GameEngine.h"
#include <iostream>



GameEngine::GameEngine(int cellSize) 
    : d_board(8, 8), 
      d_renderer(cellSize), 
      d_selectedTile(Position::NONE),
      d_cellSize(cellSize),
      d_offsetX(Config::Graphics::DEFAULT_OFFSETX),  
      d_offsetY(Config::Graphics::DEFAULT_OFFSETY),
      d_promotionMenu({BASE_PROMOTIONS}),
      d_settingsScreen(d_settings),
      d_eventManager{d_audioManager}
{
    // Appliquer les paramètres chargés
    d_settings.applySettings();
    updateBoardLayout();
    d_audioManager.setVolume(d_settings.getMusicVolume());

    d_titleScreen.setButtonClickCallback([this](){ d_audioManager.playButtonPress(); });
    d_settingsScreen.setButtonClickCallback([this](){ d_audioManager.playButtonPress(); });

    d_players.push_back(Player(PieceColor::White, Chrono::STANDARD_TIME, Chrono::STANDARD_INCREMENT));
    d_players.push_back(Player(PieceColor::Black, Chrono::STANDARD_TIME, Chrono::STANDARD_INCREMENT));
    d_currentPlayerIndex = 0;

    fillPlayerHand(d_players[0]);
    fillPlayerHand(d_players[1]);

    for(int i=0; i<5; i++) {
    d_shop.addCard(generateRandomCard()); 
}
    
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

int GameEngine::getFlippedBoardY(int boardY) const {
    if (d_players[d_currentPlayerIndex].getColor() == PieceColor::Black) {
        return d_board.getHeight() - 1 - boardY;
    }
    return boardY;
}

void GameEngine::handleInput(Position clickedPos) {
    // 1. Sécurité de base : clic hors plateau ou case gelée
    if (!d_board.isinBounds(clickedPos)) return;
    if (d_board.getTile(clickedPos).getType() == TileType::Frozen) {
        d_selectedTile = Position::NONE;
        d_currentValidMoves.clear();
        return; 
    }

    bool moveExecuted = false;
    PieceColor currentColor = d_players[d_currentPlayerIndex].getColor();

    // 2. TENTATIVE DE MOUVEMENT (Si une pièce est déjà sélectionnée)
    if (d_selectedTile.x != -1 && d_selectedTile.y != -1) {
        for (const Position& validMove : d_currentValidMoves) {
            if (clickedPos.x == validMove.x && clickedPos.y == validMove.y) { 

                bool isCapture = d_board.getTile(clickedPos).hasPiece();
                
                // --- LOGIQUE SPÉCIALE AVANT MOUVEMENT ---
                handleCastling(d_selectedTile, clickedPos);
                handleEnPassant(d_selectedTile, clickedPos);
                updateEnPassantTarget(d_selectedTile, clickedPos);

                // --- EXÉCUTION DU MOUVEMENT ---
                // movePieceWithPortal gère la téléportation et retourne la position réelle
                Position finalPos = d_board.movePieceWithPortal(d_selectedTile, clickedPos);
                if (finalPos.x == -1) continue; // Mouvement échoué

                // --- APPLIQUER LA GLISSADE SI TERRAIN GLISSANT ---
                if (d_board.isSlipperyTerrainActive()) {
                    int dirX = 0, dirY = 0;
                    if (finalPos.x > d_selectedTile.x) dirX = 1;
                    else if (finalPos.x < d_selectedTile.x) dirX = -1;
                    if (finalPos.y > d_selectedTile.y) dirY = 1;
                    else if (finalPos.y < d_selectedTile.y) dirY = -1;
                    
                    if (dirX == 0 || dirY == 0) { // Pas de glissade en diagonale
                        Position currentPos = finalPos;
                        while (true) {
                            Position nextPos = {currentPos.x + dirX, currentPos.y + dirY};
                            if (!d_board.isinBounds(nextPos) || d_board.getTile(nextPos).hasPiece() || !d_board.getTile(nextPos).isWalkable()) {
                                break;
                            }
                            std::unique_ptr<Piece> pieceToSlide = d_board.getTile(currentPos).releasePiece();
                            d_board.getTile(nextPos).setPiece(std::move(pieceToSlide));
                            currentPos = nextPos;
                        }
                        finalPos = currentPos;
                    }
                }

                // --- FEEDBACK SONORE ---
                if (isCapture) {
                    d_audioManager.playCapture();
                } else {
                    d_audioManager.playMove();
                }

                // --- PROMOTION ET FIN DE TOUR ---
                bool promotes = PromotionHandler::checkPromotion(d_board, finalPos);
                if (promotes) {
                    d_isPromoting = true;
                    d_promotionPos = finalPos;
                } else {
                    d_players[d_currentPlayerIndex].getClock().addIncrement();
                    d_players[d_currentPlayerIndex].resetTurn();
                    
                    endPlayerTurn();
                    
                    updateGameState();

                    if (d_gameState == GameState::Playing) {
                        d_eventManager.processActiveEvents(d_board);
                        d_eventManager.triggerRandomEvent(d_board);
                    }
                }

                // Reset de l'état de sélection
                d_selectedTile = {-1, -1};
                d_currentValidMoves.clear();
                moveExecuted = true;
                break;
            }
        }
    }

    // 3. SÉLECTION D'UNE PIÈCE (Si aucun mouvement n'a été validé ci-dessus)
    if (!moveExecuted) {
        const Tile& tile = d_board.getTile(clickedPos);
        
        if (tile.hasPiece() && (tile.getPiece()->getColor() == currentColor || tile.getPiece()->getType() == PieceType::Idiot)) {
            d_selectedTile = clickedPos;
            // On calcule les coups légaux (filtre les échecs au roi)
            std::vector<Position> pseudoMoves = tile.getPiece()->getValidMoves(clickedPos, d_board);
            d_currentValidMoves = filterLegalMoves(clickedPos, pseudoMoves);
        } else {
            // Clic sur case vide ou ennemie : on nettoie la sélection
            d_selectedTile = {-1, -1};
            d_currentValidMoves.clear();
        }
    }
}

void GameEngine::handleCastling(Position startPos, Position targetPos) {
    Piece* pieceToMove = d_board.getTile(startPos).getPiece();
    

    if (pieceToMove->getType() != PieceType::King || abs(targetPos.x - startPos.x) != 2) {
        return; 
    }
    
    if (targetPos.x == 6) { 
        //  Castle 
        d_board.movePiece({7, targetPos.y}, {5, targetPos.y});
        d_board.getTile({5, targetPos.y}).getPiece()->setHasMoved(true);
    } 
    else if (targetPos.x == 2) { 
        // Big Castle 
        d_board.movePiece({0, targetPos.y}, {3, targetPos.y});
        d_board.getTile({3, targetPos.y}).getPiece()->setHasMoved(true);
    }
}
void GameEngine::handleEnPassant(Position startPos, Position targetPos) {
    Piece* pieceToMove = d_board.getTile(startPos).getPiece();

    if (pieceToMove->getType() != PieceType::Pawn || targetPos != d_board.getEnPassantTarget()) {
        return;
    }

    Position enemyPawnPos = { targetPos.x, startPos.y };

    d_board.getTile(enemyPawnPos).setPiece(nullptr); 
}

void GameEngine::updateEnPassantTarget(Position startPos, Position targetPos) {
    Piece* pieceToMove = d_board.getTile(startPos).getPiece();

    if (pieceToMove->getType() == PieceType::Pawn && abs(targetPos.y - startPos.y) == 2) {
        int dir = (targetPos.y > startPos.y) ? -1 : 1;
        d_board.setEnPassantTarget({startPos.x, targetPos.y + dir});
    } else {
        d_board.setEnPassantTarget({-1, -1});
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
std::vector<Position> GameEngine::filterLegalMoves(Position startPos, const std::vector<Position>& pseudoMoves) {
    std::vector<Position> legalMoves;
    Piece* myPiece = d_board.getTile(startPos).getPiece();
    PieceColor myColor = d_players[d_currentPlayerIndex].getColor();

    bool isKing = (myPiece->getType() == PieceType::King);
    bool currentlyInCheck = d_board.isKingInCheck(myColor);

    for (const Position& targetPos : pseudoMoves) {
        if (d_board.getTile(targetPos).hasPiece() && d_board.getTile(targetPos).getPiece()->getColor() == myColor) {
            continue; 
        }

        if (isKing && abs(targetPos.x - startPos.x) == 2) {
            if (currentlyInCheck) continue; 
            int step = (targetPos.x > startPos.x) ? 1 : -1; 
            Position crossedPos = { startPos.x + step, startPos.y };

            std::unique_ptr<Piece> backupCrossed = d_board.getTile(crossedPos).releasePiece();
            std::unique_ptr<Piece> kingToMove = d_board.getTile(startPos).releasePiece();
            
            d_board.getTile(crossedPos).setPiece(std::move(kingToMove));
            bool isCrossedSafe = !d_board.isKingInCheck(myColor);
            kingToMove = d_board.getTile(crossedPos).releasePiece();
            d_board.getTile(startPos).setPiece(std::move(kingToMove));
            if (backupCrossed) d_board.getTile(crossedPos).setPiece(std::move(backupCrossed));
            
            if (!isCrossedSafe) continue; 
        }

        std::unique_ptr<Piece> backupTarget = d_board.getTile(targetPos).releasePiece();
        std::unique_ptr<Piece> pieceToMove = d_board.getTile(startPos).releasePiece();
        
        d_board.getTile(targetPos).setPiece(std::move(pieceToMove));
        bool isSafe = !d_board.isKingInCheck(myColor);
        
        pieceToMove = d_board.getTile(targetPos).releasePiece();
        d_board.getTile(startPos).setPiece(std::move(pieceToMove));
        if (backupTarget) d_board.getTile(targetPos).setPiece(std::move(backupTarget));
        
        if (isSafe) {
            legalMoves.push_back(targetPos);
        }
    }
    return legalMoves;
}

void GameEngine::run() {
    while (!WindowShouldClose() && !d_shouldQuit) {
        updateSystems(); 
        
        if (d_gameState == GameState::Playing) {
            processInput(); 
        } else if (d_gameState == GameState::TitleScreen) {
            d_titleScreen.update(GetMouseX(), GetMouseY(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
            switch (d_titleScreen.getSelectedAction()) {
                case TitleScreen::Action::Play:
                    d_gameState = GameState::Playing;
                    d_titleScreen.clearAction();
                    break;
                case TitleScreen::Action::Settings:
                    d_gameState = GameState::Settings;
                    d_settings.backupCurrentSettings(); // Sauvegarder l'état actuel
                    d_titleScreen.clearAction();
                    break;
                case TitleScreen::Action::Quit:
                    d_shouldQuit = true;
                    d_titleScreen.clearAction();
                    break;
                default:
                    break;
            }
        } else if (d_gameState == GameState::Settings) {
            d_settingsScreen.update(GetMouseX(), GetMouseY(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
            if (d_settingsScreen.getSelectedAction() == SettingsScreen::Action::Save) {
                    d_settingsScreen.clearAction();
                // Réappliquer les paramètres après sauvegarde
                d_settings.applySettings();
                updateBoardLayout();
                d_audioManager.setVolume(d_settings.getMusicVolume());
                // Retourner automatiquement à l'écran titre après sauvegarde
                d_gameState = GameState::TitleScreen;
            } else if (d_settingsScreen.getSelectedAction() == SettingsScreen::Action::Back) {
                d_gameState = GameState::TitleScreen;
                d_settingsScreen.clearAction();
                // Annuler les changements en restaurant les paramètres sauvegardés
                d_settings.restoreSettings();
                d_settings.applySettings();
                updateBoardLayout();
                d_audioManager.setVolume(d_settings.getMusicVolume());
            }
        }
        
        renderFrame(); 
        
    }
}

std::unique_ptr<Card> GameEngine::generateRandomCard() {
    int roll = GetRandomValue(0, 11); 
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
        case 9: return std::make_unique<MysteryCard>(EventRarity::Common);
        case 10: return std::make_unique<MysteryCard>(EventRarity::Rare);
        case 11: return std::make_unique<MysteryCard>(EventRarity::Epic);
        case 12: return std::make_unique<MysteryCard>(EventRarity::Legendary);
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

void GameEngine::updateBoardLayout() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    const int margin = 40;
    const int maxBoardSize = static_cast<int>(Config::Graphics::BOARD_PIXEL_SIZE);

    int boardSize = std::min({screenWidth - margin, screenHeight - margin, maxBoardSize});
    if (boardSize < 8) {
        boardSize = maxBoardSize;
    }

    int cellSize = boardSize / Config::Board::WIDTH;
    boardSize = cellSize * Config::Board::WIDTH;

    d_cellSize = cellSize;
    d_offsetX = (screenWidth - boardSize) / 2;
    d_offsetY = (screenHeight - boardSize) / 2;

    d_renderer.updateLayout((float)d_cellSize, (float)d_offsetX, (float)d_offsetY);
    d_promotionMenu.updateLayout(d_offsetX, boardSize);
}

void GameEngine::updateSystems() {
    UpdateMusicStream(d_audioManager.getMusic()); // Mise à jour de la musique
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

        d_cellSize = d_renderer.getCellSize();
        d_offsetX = d_renderer.getOffsetX();
        d_offsetY = d_renderer.getOffsetY();

        if (d_shopButton.update(mouseX, mouseY, true)) {
            d_audioManager.playButtonPress();
            d_isShopOpen = !d_isShopOpen;
            return;
        }
        if (d_toggleHandButton.update(mouseX, mouseY, true)) {
            d_audioManager.playButtonPress();
            d_isHandVisible = !d_isHandVisible;
            return; 
        }
        if (d_isShopOpen) {
            d_shopMenu.updateLayout(GetScreenWidth(), GetScreenHeight(), d_shop.getCards().size());
            // Si on clique en dehors du panneau central, on ferme la boutique
            if (!d_shopMenu.isClickInsidePanel(mouseX, mouseY)) {
                d_isShopOpen = false;
                return;
            }

            // On demande au menu sur quelle carte on a cliqué
            auto clickedIndex = d_shopMenu.getClickedCardIndex(mouseX, mouseY, d_shop.getCards().size());
            
            if (clickedIndex.has_value()) {
                // Tentative d'achat
                auto boughtCard = d_shop.buyCard(clickedIndex.value(), currentPlayer);
                if (boughtCard) {
                    currentPlayer.drawCard(std::move(boughtCard)); 
                    // d_audioManager.playBuySound(); // Optionnel : bruit d'or
                    d_shop.addCard(generateRandomCard());
                }
            }
            
            // On bloque le reste du code (pas de déplacement de pièces en arrière-plan)
            return; 
        }

        bool actionExecuted = false;
        
        if (d_isTargeting) {
            int boardX = (mouseX - d_offsetX) / d_cellSize;
            int boardY = (mouseY - d_offsetY) / d_cellSize;
            boardY = getFlippedBoardY(boardY);
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
                            currentPlayer.removeCardFromHand(d_pendingCardIndex);
                            currentPlayer.markCardAsPlayed();
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
                        currentPlayer.removeCardFromHand(d_pendingCardIndex);
                        currentPlayer.markCardAsPlayed();
                        fillPlayerHand(currentPlayer);
                        d_isTargeting = false;
                        d_pendingCardIndex = -1;
                    }
                }
            }
            actionExecuted = true; 
        }
        else if (!currentPlayer.hasPlayedCardThisTurn() && d_isHandVisible) {
            int clickedCard = d_renderer.getClickedCardIndex(currentPlayer.getHand().size(), mouseX, mouseY);

            if (clickedCard != -1) {
                if (currentPlayer.getHand()[clickedCard]->requiresTarget()) {
                    d_isTargeting = true;
                    d_pendingCardIndex = clickedCard;
                } else {
                    if (currentPlayer.playCard(clickedCard, d_board, d_eventManager)) {
                        fillPlayerHand(currentPlayer);
                    }
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
                    endPlayerTurn();
                    updateGameState();
                }
            } else {
                int boardX = (mouseX - d_offsetX) / d_cellSize;
                int boardY = (mouseY - d_offsetY) / d_cellSize;
                boardY = getFlippedBoardY(boardY);
                handleInput({boardX, boardY}); 
            }
        }
    }
    else if (d_gameState == GameState::Settings) {
        d_settingsScreen.draw();
        return;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && d_isTargeting) {
        d_isTargeting = false;
        d_pendingCardIndex = -1;
        d_firstTarget = Position::NONE;
    }
}
void GameEngine::renderFrame() {
    if (d_gameState == GameState::TitleScreen) {
        d_titleScreen.draw();
        return;
    } else if (d_gameState == GameState::Settings) {
        d_settingsScreen.draw();
        return;
    }

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
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0, 0, 0, 200 }); 
            const char* message = "";
            if (d_gameState == GameState::WhiteWins) message = "LES BLANCS GAGNENT !"; 
            if (d_gameState == GameState::BlackWins) message = "LES NOIRS GAGNENT !";  
            if (d_gameState == GameState::Stalemate) message = "PAT ! MATCH NUL !";

            int textWidth = MeasureText(message, 30);
            DrawText(message, (GetScreenWidth() / 2) - (textWidth / 2), GetScreenHeight() / 2, 30, RAYWHITE);
        }
        
        if (d_isHandVisible) {
            d_renderer.drawHands(d_players[d_currentPlayerIndex]);
        }

        if (d_isShopOpen) {
            d_shopMenu.updateLayout(GetScreenWidth(), GetScreenHeight(), d_shop.getCards().size());
        }
        d_renderer.drawShop(d_shop, d_shopMenu, d_isShopOpen);

        d_shopButton.update(GetMouseX(), GetMouseY(), false); 
        d_shopButton.draw();

        d_toggleHandButton.update(GetMouseX(), GetMouseY(), false); 
        d_toggleHandButton.draw();

        
        if (d_isTargeting) {
            DrawRectangle(0, 0, GetScreenWidth(), 40, Fade(BLACK, 0.8f));
            DrawText("MODE CIBLAGE : Cliquez sur une case du plateau (Clic droit pour annuler)", 150, 10, 20, YELLOW);
        }
        
    EndDrawing();
}

void GameEngine::endPlayerTurn() {
    d_turnCount++;
    
    // Tous les 2 tours de joueurs (après Blanc et Noir), le canard joue
    if (d_turnCount % 2 == 0) {
        Position duckPos = {-1, -1};
        for (int y = 0; y < d_board.getHeight(); ++y) {
            for (int x = 0; x < d_board.getWidth(); ++x) {
                Position pos = {x, y};
                const Tile& tile = d_board.getTile(pos);
                if (tile.hasPiece() && tile.getPiece()->getType() == PieceType::Duck) {
                    duckPos = pos;
                    break;
                }
            }
            if (duckPos.x != -1) break;
        }
        
        if (duckPos.x != -1) {
            Piece* duckPiece = d_board.getTile(duckPos).getPiece();
            std::vector<Position> validMoves = duckPiece->getValidMoves(duckPos, d_board);
            
            if (!validMoves.empty()) {
                int randomIdx = GetRandomValue(0, validMoves.size() - 1);
                Position newPos = validMoves[randomIdx];
                
                if (d_board.getTile(newPos).hasPiece()) d_board.getTile(newPos).releasePiece();
                
                std::unique_ptr<Piece> duck = d_board.getTile(duckPos).releasePiece();
                d_board.getTile(newPos).setPiece(std::move(duck));
                
                d_audioManager.playMove();
            }
        }
        d_currentPlayerIndex = (d_currentPlayerIndex + 1) % d_players.size();
    } else {
        // Sinon, on passe simplement au joueur suivant
        d_currentPlayerIndex = (d_currentPlayerIndex + 1) % d_players.size();
    }
}

void GameEngine::applySlipperyTerrain(Position startPos, Position targetPos) {
    // Fonction conservée pour correspondre à la déclaration dans GameEngine.h,
    // mais la vraie logique a été intégrée directement dans handleInput()
    // car elle a besoin de modifier finalPos par référence.
}
