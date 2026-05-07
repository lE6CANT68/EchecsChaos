#include "GameEngine.h"



GameEngine::GameEngine(int cellSize) 
    : d_board(8, 8), 
      d_renderer(cellSize), 
      d_selectedTile({-1, -1}),
      d_cellSize(cellSize),
      d_offsetX(Renderer::DEFAULT_OFFSETX),  
      d_offsetY(Renderer::DEFAULT_OFFSETX),
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
    //Pawn placement for both
    for (int i = 0; i < 8; ++i) {
        d_board.getTile({i, 1}).setPiece(std::make_unique<Pawn>(PieceColor::Black));
        d_board.getTile({i, 6}).setPiece(std::make_unique<Pawn>(PieceColor::White));
    }

    // Black pieces
    d_board.getTile({0, 0}).setPiece(std::make_unique<Rook>(PieceColor::Black));
    d_board.getTile({1, 0}).setPiece(std::make_unique<Knight>(PieceColor::Black));
    d_board.getTile({2, 0}).setPiece(std::make_unique<Bishop>(PieceColor::Black));
    d_board.getTile({3, 0}).setPiece(std::make_unique<Queen>(PieceColor::Black));
    d_board.getTile({4, 0}).setPiece(std::make_unique<King>(PieceColor::Black));
    d_board.getTile({5, 0}).setPiece(std::make_unique<Bishop>(PieceColor::Black));
    d_board.getTile({6, 0}).setPiece(std::make_unique<Knight>(PieceColor::Black));
    d_board.getTile({7, 0}).setPiece(std::make_unique<Rook>(PieceColor::Black));

    // White pieces
    d_board.getTile({0, 7}).setPiece(std::make_unique<Rook>(PieceColor::White));
    d_board.getTile({1, 7}).setPiece(std::make_unique<Knight>(PieceColor::White));
    d_board.getTile({2, 7}).setPiece(std::make_unique<Bishop>(PieceColor::White));
    d_board.getTile({3, 7}).setPiece(std::make_unique<Queen>(PieceColor::White));
    d_board.getTile({4, 7}).setPiece(std::make_unique<King>(PieceColor::White));
    d_board.getTile({5, 7}).setPiece(std::make_unique<Bishop>(PieceColor::White));
    d_board.getTile({6, 7}).setPiece(std::make_unique<Knight>(PieceColor::White));
    d_board.getTile({7, 7}).setPiece(std::make_unique<Rook>(PieceColor::White));
}

void GameEngine::handleInput(Position clickedPos) {
    if (!d_board.isinBounds(clickedPos)) return;

    if (d_board.getTile(clickedPos).getType() == TileType::Frozen) {
        d_selectedTile = {-1, -1};
        d_currentValidMoves.clear();
        return; 
    }
    // ----------------------------------------------------

    bool moveExecuted = false;

    PieceColor currentColor = d_players[d_currentPlayerIndex].getColor();

    if (d_selectedTile.x != -1 && d_selectedTile.y != -1) {
        for (const Position& validMove : d_currentValidMoves) {
            if (clickedPos.x == validMove.x && clickedPos.y == validMove.y) { 

                bool isCapture = d_board.getTile(clickedPos).hasPiece();
                
                handleCastling(d_selectedTile, clickedPos);
                handleEnPassant(d_selectedTile, clickedPos);
                updateEnPassantTarget(d_selectedTile, clickedPos);

                d_board.movePiece(d_selectedTile, clickedPos);
                d_board.getTile(clickedPos).getPiece()->setHasMoved(true);

                if (isCapture) {
                    d_audioManager.playCapture();
                    // Plus tard, on ajoutera les points ici !
                    // d_players[d_currentPlayerIndex].addPoints(pieceValue);
                } else {
                    d_audioManager.playMove();
                }

                bool promotes = checkPromotion(clickedPos);
                if (!promotes) {

                    d_players[d_currentPlayerIndex].getClock().addIncrement();
                    d_players[d_currentPlayerIndex].resetTurn(); 
                    

                    d_currentPlayerIndex = (d_currentPlayerIndex + 1) % d_players.size();
                    
                    updateGameState();

                    if (d_gameState == GameState::Playing) {
                        d_eventManager.processActiveEvents(d_board);
                        d_eventManager.triggerRandomEvent(d_board);
                    }
                }

                d_selectedTile = {-1, -1};
                d_currentValidMoves.clear();
                moveExecuted = true;
                break;
            }
        }
    }

    if (!moveExecuted) {
        const Tile& tile = d_board.getTile(clickedPos);
        if (tile.hasPiece() && (tile.getPiece()->getColor() == currentColor || tile.getPiece()->getType() == PieceType::Idiot)) {
            d_selectedTile = clickedPos;
            std::vector<Position> pseudoMoves = tile.getPiece()->getValidMoves(clickedPos, d_board);
            d_currentValidMoves = filterLegalMoves(clickedPos, pseudoMoves);
            
        } else {
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

        int skippedY = (startPos.y + targetPos.y) / 2;
        d_board.setEnPassantTarget({ startPos.x, skippedY });
    } 
    else {
        d_board.setEnPassantTarget({-1, -1});
    }
}
bool GameEngine::checkPromotion(Position targetPos) {
    Piece* piece = d_board.getTile(targetPos).getPiece();
    
    if (piece->getType() == PieceType::Pawn) {
        if ((piece->getColor() == PieceColor::White && targetPos.y == 0) ||
            (piece->getColor() == PieceColor::Black && targetPos.y == 7)) {
            
            d_isPromoting = true;
            d_promotionPos = targetPos;
            return true;
        }
    }
    return false;
}
void GameEngine::handlePromotion(int mouseX, int mouseY) {
    int boxSize = 100;
    int startX = 200;
    int startY = 350;
    if (mouseY < startY || mouseY > startY + boxSize || mouseX < startX || mouseX > startX + (4 * boxSize)) {
        return; 
    }

    PieceColor color = d_board.getTile(d_promotionPos).getPiece()->getColor();
    int choice = (mouseX - startX) / boxSize;

    if (choice == 0) d_board.getTile(d_promotionPos).setPiece(std::make_unique<Queen>(color));
    else if (choice == 1) d_board.getTile(d_promotionPos).setPiece(std::make_unique<Rook>(color));
    else if (choice == 2) d_board.getTile(d_promotionPos).setPiece(std::make_unique<Knight>(color));
    else if (choice == 3) d_board.getTile(d_promotionPos).setPiece(std::make_unique<Bishop>(color));
    d_isPromoting = false;
    d_promotionPos = {-1, -1};

    d_players[d_currentPlayerIndex].getClock().addIncrement();

    d_players[d_currentPlayerIndex].resetTurn();

    d_currentPlayerIndex = (d_currentPlayerIndex + 1) % d_players.size();
    updateGameState();
}

void GameEngine::updateGameState() {
    bool hasAnyLegalMove = false;
    PieceColor currentColor = d_players[d_currentPlayerIndex].getColor();

    for (int x = 0; x < d_board.getWidth(); ++x) {
        for (int y = 0; y < d_board.getHeight(); ++y) {
            
            const Tile& tile = d_board.getTile({x, y});

            // On utilise currentColor au lieu du vieux d_currentTurn
            if (tile.hasPiece() && tile.getPiece()->getColor() == currentColor) {

                std::vector<Position> pseudoMoves = tile.getPiece()->getValidMoves({x, y}, d_board);
                std::vector<Position> legalMoves = filterLegalMoves({x, y}, pseudoMoves);

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

        std::unique_ptr<Piece> pieceToReturn = d_board.getTile(targetPos).releasePiece();
        d_board.getTile(startPos).setPiece(std::move(pieceToReturn));

        if (backupTarget) {
            d_board.getTile(targetPos).setPiece(std::move(backupTarget));
        }

        if (isSafe) {
            legalMoves.push_back(targetPos); 
        }
    }

    return legalMoves;
}
void GameEngine::run() {
    while (!WindowShouldClose()) {
        d_audioManager.updateMusic();
        d_eventManager.update();

        Player& currentPlayer = d_players[d_currentPlayerIndex];
        PieceColor currentColor = currentPlayer.getColor();

        if (d_gameState == GameState::Playing) {
            if (!d_isPromoting) { 
                currentPlayer.getClock().update();
                
                if (currentPlayer.getClock().isZero()) {
                    d_gameState = (currentColor == PieceColor::White) ? GameState::BlackWins : GameState::WhiteWins; 
                    d_audioManager.playGameOver();
                }
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();
            bool actionExecuted = false;

            // --- 1. SI ON EST DÉJÀ EN MODE CIBLAGE ---
            if (d_isTargeting) {
                int boardX = (mouseX - d_offsetX) / d_cellSize;
                int boardY = (mouseY - d_offsetY) / d_cellSize;
                Position targetPos = {boardX, boardY};

                // On vérifie que le clic est sur le plateau, sur une case vide et normale
                if (d_board.isinBounds(targetPos)) {
                    currentPlayer.playCard(d_pendingCardIndex, d_board, d_eventManager, targetPos);
                    fillPlayerHand(currentPlayer);
                    d_isTargeting = false;
                    d_pendingCardIndex = -1;
                }
                actionExecuted = true; // On a géré le clic, on bloque le reste
            }

            // --- 2. SI ON CLIQUE SUR UNE CARTE ---
            else if (!currentPlayer.hasPlayedCardThisTurn()) {
                int clickedCard = d_renderer.getClickedCardIndex(d_currentPlayerIndex, currentPlayer.getHand().size(), mouseX, mouseY);

                if (clickedCard != -1) {
                    // La carte a-t-elle besoin d'une cible ?
                    if (currentPlayer.getHand()[clickedCard]->requiresTarget()) {
                        // OUI : On active le mode ciblage !
                        d_isTargeting = true;
                        d_pendingCardIndex = clickedCard;
                    } else {
                        // NON : On la joue direct (ex: Météorite, Sablier)
                        currentPlayer.playCard(clickedCard, d_board, d_eventManager);
                        fillPlayerHand(currentPlayer);
                    }
                    actionExecuted = true;
                }
            }

            // --- 3. GESTION NORMALE DE L'ÉCHIQUIER ---
            if (!actionExecuted && !d_isTargeting) {
                if (d_isPromoting) {
                    handlePromotion(mouseX, mouseY);
                } else {
                    int boardX = (mouseX - d_offsetX) / d_cellSize;
                    int boardY = (mouseY - d_offsetY) / d_cellSize;
                    handleInput({boardX, boardY});
                }
            }
        }

    // ASTUCE : Clic Droit pour annuler le ciblage si le joueur change d'avis !
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && d_isTargeting) {
        d_isTargeting = false;
        d_pendingCardIndex = -1;
    }
        }
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
            
            Position checkedKingPos = {-1, -1};
            if (d_board.isKingInCheck(currentColor)) {
                checkedKingPos = d_board.getKingPosition(currentColor);
            }
            d_renderer.draw(d_board, d_selectedTile, d_currentValidMoves, currentColor,checkedKingPos);
            d_renderer.drawChrono(
                d_players[0].getClock().getFormattedTime(), 
                d_players[1].getClock().getFormattedTime(), 
                currentColor,
                d_offsetX,  
                d_offsetY  
            );

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
                DrawRectangle(0, 0, 800, 800, { 0, 0, 0, 200 }); 
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
}
std::unique_ptr<Card> GameEngine::generateRandomCard() {
    int roll = GetRandomValue(0, 5); 
    
    switch (roll) {
        case 0: return std::make_unique<MeteoriteCard>();
        case 1: return std::make_unique<TimeCard>();
        case 2: return std::make_unique<BlockCard>();
        case 3: return std::make_unique<PawnSprintCard>();
        case 4: return std::make_unique<FreezeCard>();
        case 5: return std::make_unique<FogCard>();
        default: return std::make_unique<MeteoriteCard>(); 
    }
}


void GameEngine::fillPlayerHand(Player& player) {
    while (player.getHand().size() < 3) {
        player.drawCard(generateRandomCard());
    }
}

