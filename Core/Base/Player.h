#pragma once
#include <vector>
#include <memory>
#include "../Time/Chrono.h" 
#include "Card.h"

class Board;
class EventManager;

class Player {
public:
    Player(PieceColor color, float startTime, float increment);

    
    PieceColor getColor() const;
    Chrono& getClock();

    int getPoints() const;
    void addPoints(int amount);
    bool spendPoints(int amount);

    const std::vector<std::unique_ptr<Card>>& getHand() const;
    void drawCard(std::unique_ptr<Card> newCard);
    void playCard(int handIndex, Board& board, EventManager& eventManager,Position target = {-1, -1});
    bool hasPlayedCardThisTurn() const;
    void resetTurn();

    void addScore(int points);
    int getScore() const;
    void spendScore(int amount);

private:
    PieceColor d_color;
    Chrono d_clock;
    int d_points;
    bool d_hasPlayedCardThisTurn;
    std::vector<std::unique_ptr<Card>> d_hand; 
    int d_score = 0;
};