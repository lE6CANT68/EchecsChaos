#include "GameEngine.h"
#include <algorithm>
#include <iostream>
#include "../Network/Network.h"
#include "../../json.hpp"



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

    d_boardHistory.clear();
    d_boardHistory.push_back(d_board.toString(PieceColor::White));

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
    if (!d_board.isinBounds(clickedPos)) return;
    if (d_board.getTile(clickedPos).getType() == TileType::Frozen) {
        d_selectedTile = Position::NONE;
        d_currentValidMoves.clear();
        return; 
    }

    bool moveExecuted = false;
    PieceColor currentColor = d_players[d_currentPlayerIndex].getColor();
    if (d_selectedTile.x != -1 && d_selectedTile.y != -1) {
        for (const Position& validMove : d_currentValidMoves) {
            if (clickedPos.x == validMove.x && clickedPos.y == validMove.y) { 

                bool isCapture = d_board.getTile(clickedPos).hasPiece();
                if (isCapture) {
                    PieceType capturedType = d_board.getTile(clickedPos).getPiece()->getType();
                    int earnedPoints = ScoreHandler::calculateCaptureScore(capturedType);
                    d_players[d_currentPlayerIndex].addScore(earnedPoints);
                }
                handleCastling(d_selectedTile, clickedPos);
                handleEnPassant(d_selectedTile, clickedPos);
                updateEnPassantTarget(d_selectedTile, clickedPos);

                Position finalPos = d_board.movePieceWithPortal(d_selectedTile, clickedPos);
                if (finalPos.x == -1) continue; 


                if (d_board.isSlipperyTerrainActive()) {
                    int dirX = 0, dirY = 0;
                    if (finalPos.x > d_selectedTile.x) dirX = 1;
                    else if (finalPos.x < d_selectedTile.x) dirX = -1;
                    if (finalPos.y > d_selectedTile.y) dirY = 1;
                    else if (finalPos.y < d_selectedTile.y) dirY = -1;
                    
                    if (dirX == 0 || dirY == 0) { 
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

                if (isCapture) {
                    d_audioManager.playCapture();
                } else {
                    d_audioManager.playMove();
                }
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
        
        if (d_gameState == GameState::Playing || d_gameState == GameState::WAITING_FOR_AI ||
            d_gameState == GameState::WhiteWins || d_gameState == GameState::BlackWins ||
            d_gameState == GameState::Stalemate || d_gameState == GameState::WAITING_FOR_ANALYSIS ||
            d_gameState == GameState::SHOW_ANALYSIS) {
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
                    d_settings.backupCurrentSettings(); 
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
                d_settings.applySettings();
                updateBoardLayout();
                d_audioManager.setVolume(d_settings.getMusicVolume());
                d_gameState = GameState::TitleScreen;
            } else if (d_settingsScreen.getSelectedAction() == SettingsScreen::Action::Back) {
                d_gameState = GameState::TitleScreen;
                d_settingsScreen.clearAction();
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
    d_audioManager.updateMusicStream();
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
    if (d_gameState == GameState::WAITING_FOR_ANALYSIS) {
        if (d_aiAnalysisFuture.valid() && 
            d_aiAnalysisFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            
            std::string reponseTexte = d_aiAnalysisFuture.get();
            std::cout << "Analyse IA : " << reponseTexte << std::endl;
            
            try {
                auto jsonResponse = nlohmann::json::parse(reponseTexte);
                d_analysisMoves.clear();
                if (jsonResponse.is_array()) {
                    for (auto& item : jsonResponse) {
                        AnalysisMove am;
                        am.turn = item.value("tour", 0);
                        am.comment = item.value("commentaire", "Pas de commentaire");
                        if (am.turn >= 0 && am.turn < (int)d_boardHistory.size()) {
                            am.fen = d_boardHistory[am.turn];
                        } else {
                            am.fen = d_boardHistory.empty() ? "" : d_boardHistory.back();
                        }
                        d_analysisMoves.push_back(am);
                    }
                }
            } catch(const std::exception& e) {
                std::cerr << "[ERREUR IA ANALYSE] JSON invalide : " << e.what() << std::endl;
            }

            d_currentAnalysisIndex = 0;
            if (!d_analysisMoves.empty()) {
                d_analysisBoard.loadFromString(d_analysisMoves[0].fen);
            }
            d_gameState = GameState::SHOW_ANALYSIS; 
        }
        return;
    }

    if (d_gameState == GameState::SHOW_ANALYSIS) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        bool isLeftClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (isLeftClick) {
            if (d_nextAnalysisButton.update(mouseX, mouseY, true)) {
                if (d_currentAnalysisIndex < (int)d_analysisMoves.size() - 1) {
                    d_currentAnalysisIndex++;
                    d_analysisBoard.loadFromString(d_analysisMoves[d_currentAnalysisIndex].fen);
                    d_audioManager.playButtonPress();
                }
            } else if (d_prevAnalysisButton.update(mouseX, mouseY, true)) {
                if (d_currentAnalysisIndex > 0) {
                    d_currentAnalysisIndex--;
                    d_analysisBoard.loadFromString(d_analysisMoves[d_currentAnalysisIndex].fen);
                    d_audioManager.playButtonPress();
                }
            } else if (d_closeAnalysisButton.update(mouseX, mouseY, true)) {
                d_audioManager.playBaseMusic();
                d_gameState = GameState::TitleScreen;
                d_audioManager.playButtonPress();
            }
        }
        return;
    }

    if (d_gameState == GameState::WhiteWins || d_gameState == GameState::BlackWins || d_gameState == GameState::Stalemate) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && d_analyzeButton.update(mouseX, mouseY, true)) {
            d_audioManager.playButtonPress();
            
            std::string prompt = "[Role systeme]\nTu es un expert cynique et moqueur en echecs chaotiques.\nVoici l'historique des positions (FEN) de la partie :\n";
            for (size_t i = 0; i < d_boardHistory.size(); ++i) {
                prompt += "Tour " + std::to_string(i) + ": " + d_boardHistory[i] + "\n";
            }
            prompt += "\nFais une analyse des 3 pires coups joues et fou toi de la gueule des joueur au maximum(et donne le numero du tour associe). Format de Reponse STRICT au format JSON (sans markdown, sans texte supplementaire) :\n";
            prompt += "[ { \"tour\": X, \"commentaire\": \"...\" } ]\n";
            
            d_aiAnalysisFuture = std::async(std::launch::async, askAIAnalysis, prompt);
            d_audioManager.playGeminiWaitMusic();
            d_gameState = GameState::WAITING_FOR_ANALYSIS;
        }
        return;
    }

    if (d_gameState == GameState::WAITING_FOR_AI) {
        if (d_aiResponseFuture.valid() && 
            d_aiResponseFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            
            std::string reponseTexte = d_aiResponseFuture.get();
            std::cout << "Le Dieu du Chaos a ordonne : " << reponseTexte << std::endl;
            
            try {
                auto jsonResponse = nlohmann::json::parse(reponseTexte);
                if (jsonResponse.contains("start_x") && jsonResponse.contains("start_y") && 
                    jsonResponse.contains("target_x") && jsonResponse.contains("target_y")) {
                    
                    Position startPos = {jsonResponse["start_x"].get<int>(), jsonResponse["start_y"].get<int>()};
                    Position targetPos = {jsonResponse["target_x"].get<int>(), jsonResponse["target_y"].get<int>()};
                    
                    
                    d_selectedTile = {-1, -1};
                    d_currentValidMoves.clear();
                    handleInput(startPos);
                    handleInput(targetPos);
                }
            } catch(const std::exception& e) {
                std::cerr << "[ERREUR IA] Ordre invalide : " << e.what() << std::endl;
            }

            d_audioManager.playBaseMusic();
            d_gameState = GameState::Playing; 
        }
        return;
    }

    Player& currentPlayer = d_players[d_currentPlayerIndex];
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    bool isLeftClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (isLeftClick && d_aiButton.update(mouseX, mouseY, true)) {
        d_audioManager.playButtonPress();
        d_isAIPromptOpen = !d_isAIPromptOpen;
        return;
    }

    if (d_isAIPromptOpen) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (d_aiPromptText.length() < 100)) {
                d_aiPromptText += static_cast<char>(key);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !d_aiPromptText.empty()) {
            d_aiPromptText.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER) && !d_aiPromptText.empty()) {
            std::string fenPlateau = d_board.toString(d_players[d_currentPlayerIndex].getColor());
            std::string etatChaos = d_board.getChaosStateString();
            
            std::string prompt = "[Role systeme]\n"
                                 "Tu es l'IA d'un jeu d'echecs CHAOTIQUE. Ton role est de traduire l'ordre naturel du joueur en un coup d'echecs valide.\n\n"
                                 "[Etat actuel du Jeu]\n"
                                 "Plateau (FEN) : " + fenPlateau + "\n\n"
                                 "[Evenements Chaotiques Actuels]\n"
                                 + etatChaos + "\n"
                                 "[Pieces Speciales du Jeu]\n"
                                 "- Chameleon (C) : Mime les deplacements de la derniere piece deplacee. Si c'est un Chameleon, agit comme Reine.\n"
                                 "- Duck (D) : Se deplace comme un Cavalier et peut capturer les pieces adverses.\n"
                                 "- Idiot (I) : Se deplace d'une case dans toutes les directions, mais ne peut pas capturer le Roi.\n"
                                 "- PionDebile (X) : Avance en diagonale sans prendre, capture uniquement droit devant.\n\n"
                                 "[Coups Legaux Disponibles]\n";

            
                                 
            PieceColor currentColor = d_players[d_currentPlayerIndex].getColor();
            for (int x = 0; x < d_board.getWidth(); ++x) {
                for (int y = 0; y < d_board.getHeight(); ++y) {
                    Position p = {x, y};
                    const Tile& tile = d_board.getTile(p);
                    if (tile.hasPiece() && tile.getPiece()->getColor() == currentColor) {
                        std::string pieceName = "Piece";
                        switch (tile.getPiece()->getType()) {
                            case PieceType::Pawn: pieceName = "Pion"; break;
                            case PieceType::Rook: pieceName = "Tour"; break;
                            case PieceType::Knight: pieceName = "Cavalier"; break;
                            case PieceType::Bishop: pieceName = "Fou"; break;
                            case PieceType::Queen: pieceName = "Reine"; break;
                            case PieceType::King: pieceName = "Roi"; break;
                            default: break;
                        }
                        
                        std::vector<Position> pseudoMoves = tile.getPiece()->getValidMoves(p, d_board);
                        std::vector<Position> legalMoves = filterLegalMoves(p, pseudoMoves);
                        for (const auto& move : legalMoves) {
                            prompt += "- " + pieceName + " de (" + std::to_string(p.x) + "," + std::to_string(p.y) + ") vers (" + std::to_string(move.x) + "," + std::to_string(move.y) + ")\n";
                        }
                    }
                }
            }
            prompt += "\n[Ordre du Joueur]\n\"" + d_aiPromptText + "\"\n";
            prompt += "\n[Format de Reponse STRICT]\nReponds UNIQUEMENT au format JSON (sans markdown, sans texte supplementaire) :\n{ \"start_x\": X, \"start_y\": Y, \"target_x\": X, \"target_y\": Y }\n";
            std::cout<<prompt<<std::endl;
            d_aiResponseFuture = std::async(std::launch::async, askAI, prompt);
            d_audioManager.playGeminiWaitMusic();
            d_gameState = GameState::WAITING_FOR_AI;
            d_isAIPromptOpen = false;
            d_aiPromptText.clear();
        }
        return; 
    }

    if (isLeftClick) {

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
            if (!d_shopMenu.isClickInsidePanel(mouseX, mouseY)) {
                d_isShopOpen = false;
                return;
            }
            auto clickedIndex = d_shopMenu.getClickedCardIndex(mouseX, mouseY, d_shop.getCards().size());
            
            if (clickedIndex.has_value()) {
                auto boughtCard = d_shop.buyCard(clickedIndex.value(), currentPlayer);
                if (boughtCard) {
                    currentPlayer.drawCard(std::move(boughtCard)); 
                    d_shop.addCard(generateRandomCard());
                }
            }
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
    } else if (d_gameState == GameState::WAITING_FOR_ANALYSIS) {
        BeginDrawing();
        ClearBackground({10, 10, 30, 255});

        float animTime = GetTime();
        int centerX = GetScreenWidth() / 2;
        int centerY = GetScreenHeight() / 2;
        float pulse = 1.0f + 0.15f * sinf(animTime * 3.5f);

        DrawCircleGradient(centerX, centerY, 140 * pulse, {30, 30, 90, 180}, {8, 8, 40, 10});
        DrawCircleLines(centerX, centerY, 140 * pulse, SKYBLUE);
        DrawCircleLines(centerX, centerY, 110 * pulse, LIGHTGRAY);
        DrawCircleLines(centerX, centerY, 80 * pulse, {255, 215, 0, 120});

        DrawText("ANALYSE DE FIN DE PARTIE EN COURS", centerX - 310, centerY - 90, 28, GOLD);
        DrawText("Patiente pendant la génération du rapport final.", centerX - 300, centerY - 50, 20, LIGHTGRAY);

        for (int i = 0; i < 5; ++i) {
            float angle = animTime * 1.8f + i * (2 * PI / 5.0f);
            float x = centerX + cosf(angle) * (90.0f + 10.0f * sinf(animTime * 0.8f + i));
            float y = centerY + sinf(angle) * (90.0f + 10.0f * cosf(animTime * 0.8f + i));
            float alpha = 0.4f + 0.6f * ((i + (int)(animTime * 2.0f)) % 5) / 4.0f;
            DrawCircle((int)x, (int)y, 10, {180, 220, 255, (unsigned char)(alpha * 255)});
        }
        EndDrawing();
        return;
    } else if (d_gameState == GameState::SHOW_ANALYSIS) {
        BeginDrawing();
        DrawRectangleGradientEx({0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()}, {8, 8, 24, 255}, {18, 18, 60, 255}, {12, 12, 40, 255}, {25, 20, 80, 255});

        int leftPanelX = 40;
        int leftPanelY = 60;
        int leftPanelW = GetScreenWidth() / 2 - 60;
        int leftPanelH = GetScreenHeight() - 120;

        int rightPanelX = GetScreenWidth() / 2 + 20;
        int rightPanelY = 60;
        int rightPanelW = GetScreenWidth() / 2 - 60;
        int rightPanelH = GetScreenHeight() - 120;

        DrawRectangle(leftPanelX, leftPanelY, leftPanelW, leftPanelH, Fade(BLACK, 0.75f));
        DrawRectangle(rightPanelX, rightPanelY, rightPanelW, rightPanelH, Fade(BLACK, 0.65f));
        DrawRectangleLines(leftPanelX, leftPanelY, leftPanelW, leftPanelH, {220, 200, 80, 255});
        DrawRectangleLines(rightPanelX, rightPanelY, rightPanelW, rightPanelH, {90, 180, 255, 220});

        DrawText("RAPPORT D'ANALYSE DU CHAOS", leftPanelX + 24, leftPanelY + 20, 28, GOLD);
        DrawText("Plateau final et commentaire", rightPanelX + 24, rightPanelY + 20, 22, SKYBLUE);

        if (!d_analysisMoves.empty()) {
            float oldCellSize = d_renderer.getCellSize();
            int oldOffsetX = d_renderer.getOffsetX();
            int oldOffsetY = d_renderer.getOffsetY();

            int boardMaxWidth = leftPanelW - 40;
            int boardMaxHeight = leftPanelH - 120;
            float analysisCellSize = std::min(boardMaxWidth / 8.0f, boardMaxHeight / 8.0f);
            int boardOffsetX = leftPanelX + (leftPanelW - (int)(analysisCellSize * 8)) / 2;
            int boardOffsetY = leftPanelY + 80;
            d_renderer.updateLayout(analysisCellSize, boardOffsetX, boardOffsetY);
            d_renderer.draw(d_analysisBoard, Position::NONE, {}, PieceColor::White, 0, 0, Position::NONE);
            d_renderer.updateLayout(oldCellSize, oldOffsetX, oldOffsetY);

            int textX = rightPanelX + 30;
            int textY = rightPanelY + 70;
            int textW = rightPanelW - 60;
            DrawText("Commentaire :", textX, textY, 22, WHITE);
            textY += 40;

            std::string text = "Tour " + std::to_string(d_analysisMoves[d_currentAnalysisIndex].turn) + " : " + d_analysisMoves[d_currentAnalysisIndex].comment;
            std::string currentLine;
            size_t lastSpace = 0;
            for (size_t i = 0; i < text.size(); ++i) {
                currentLine += text[i];
                if (text[i] == ' ') lastSpace = currentLine.size() - 1;
                if (MeasureText(currentLine.c_str(), 20) > textW) {
                    if (lastSpace > 0 && lastSpace < currentLine.size() - 1) {
                        std::string toDraw = currentLine.substr(0, lastSpace);
                        DrawText(toDraw.c_str(), textX, textY, 20, RAYWHITE);
                        currentLine = currentLine.substr(lastSpace + 1);
                    } else {
                        DrawText(currentLine.c_str(), textX, textY, 20, RAYWHITE);
                        currentLine.clear();
                    }
                    textY += 28;
                    lastSpace = 0;
                }
            }
            if (!currentLine.empty()) {
                DrawText(currentLine.c_str(), textX, textY, 20, RAYWHITE);
            }

            int buttonY = rightPanelY + rightPanelH - 70;
            d_prevAnalysisButton.setPosition(rightPanelX + 40, buttonY);
            d_nextAnalysisButton.setPosition(rightPanelX + 220, buttonY);
            d_closeAnalysisButton.setPosition(rightPanelX + rightPanelW - d_closeAnalysisButton.getWidth() - 40, buttonY);

            d_prevAnalysisButton.update(GetMouseX(), GetMouseY(), false);
            d_nextAnalysisButton.update(GetMouseX(), GetMouseY(), false);
            d_closeAnalysisButton.update(GetMouseX(), GetMouseY(), false);

            d_prevAnalysisButton.draw();
            d_nextAnalysisButton.draw();
            d_closeAnalysisButton.draw();
        } else {
            DrawText("L'IA n'a pas pu analyser la partie.", rightPanelX + 30, rightPanelY + 120, 24, RAYWHITE);
            d_closeAnalysisButton.update(GetMouseX(), GetMouseY(), false);
            d_closeAnalysisButton.draw();
        }
        EndDrawing();
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

        int bottomScore = (currentColor == PieceColor::White) ? d_players[0].getScore() : d_players[1].getScore(); 
        int topScore = (currentColor == PieceColor::White) ? d_players[1].getScore() : d_players[0].getScore();

        const char* bottomChrono = (currentColor == PieceColor::White) ? getPlayerTimeString(0) : getPlayerTimeString(1);
        const char* topChrono = (currentColor == PieceColor::White) ? getPlayerTimeString(1) : getPlayerTimeString(0);

        d_renderer.draw(d_board, d_selectedTile, d_currentValidMoves, currentColor, bottomScore, topScore, checkedKingPos);
        d_renderer.drawChrono(bottomChrono, topChrono, currentColor, d_offsetX, d_offsetY);

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

            if (d_gameState == GameState::WhiteWins || d_gameState == GameState::BlackWins || d_gameState == GameState::Stalemate) {
                int textWidth = MeasureText(message, 30);
                DrawText(message, (GetScreenWidth() / 2) - (textWidth / 2), GetScreenHeight() / 2 - 50, 30, RAYWHITE);
                
                d_analyzeButton.update(GetMouseX(), GetMouseY(), false);
                d_analyzeButton.draw();
            }
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

                d_aiButton.update(GetMouseX(), GetMouseY(), false);
        d_aiButton.draw();
        
        if (d_isAIPromptOpen) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 150});
            
            int boxWidth = 600;
            int boxHeight = 100;
            int boxX = (GetScreenWidth() - boxWidth) / 2;
            int boxY = (GetScreenHeight() - boxHeight) / 2;
            
            DrawRectangle(boxX, boxY, boxWidth, boxHeight, LIGHTGRAY);
            DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
            
            DrawText("Que dois-je faire ? (Appuyez sur Entree pour valider)", boxX + 10, boxY + 10, 20, DARKGRAY);
            DrawText(d_aiPromptText.c_str(), boxX + 10, boxY + 50, 20, MAROON);
        }

        
        if (d_isTargeting) {
            DrawRectangle(0, 0, GetScreenWidth(), 40, Fade(BLACK, 0.8f));
            DrawText("MODE CIBLAGE : Cliquez sur une case du plateau (Clic droit pour annuler)", 150, 10, 20, YELLOW);
        }
        
        if (d_gameState == GameState::WAITING_FOR_AI) {
            float animTime = GetTime();
            int dotCount = ((int)(animTime * 2.0f) % 3) + 1;
            std::string dots(dotCount, '.');
            std::string waitingText = std::string("Invocation de la magie noire en cours") + dots;
            DrawText(waitingText.c_str(), 10, 10, 20, RED);
            DrawText("Patientez, le jeu n'est pas fig\xC3\xA9.", 10, 40, 20, ORANGE);

            Vector2 center = { (float)GetScreenWidth() - 80.0f, 80.0f };
            const int circleCount = 5;
            const float radius = 42.0f;
            for (int i = 0; i < circleCount; ++i) {
                float angle = animTime * 3.0f + i * (2 * PI / circleCount);
                float x = center.x + cosf(angle) * radius;
                float y = center.y + sinf(angle) * radius;
                float alpha = 0.25f + 0.75f * ((i + (int)(animTime * 2.0f)) % circleCount) / (float)(circleCount - 1);
                Color c = { 255, 255, 255, (unsigned char)(alpha * 255) };
                DrawCircle((int)x, (int)y, 8, c);
            }
        }
        
    EndDrawing();
}

void GameEngine::endPlayerTurn() {
    d_turnCount++;

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
    d_boardHistory.push_back(d_board.toString(d_players[d_currentPlayerIndex].getColor()));
}

void GameEngine::applySlipperyTerrain(Position startPos, Position targetPos) {
}
