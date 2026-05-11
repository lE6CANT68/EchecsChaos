#include "Player.h"



Player::Player(PieceColor color, float startTime, float increment) 
    : d_color(color), d_clock(startTime, increment), d_points(0), d_hasPlayedCardThisTurn(false) {}

PieceColor Player::getColor() const { 
    return d_color; 
}

Chrono& Player::getClock() { 
    return d_clock; 
}

int Player::getPoints() const { 
    return d_points; 
}

void Player::addPoints(int amount) { 
    d_points += amount; 
}

bool Player::spendPoints(int amount) {
    if (d_points >= amount) {
        d_points -= amount;
        return true;
    }
    return false;
}

const std::vector<std::unique_ptr<Card>>& Player::getHand() const { 
    return d_hand; 
}

void Player::drawCard(std::unique_ptr<Card> newCard) {
    d_hand.push_back(std::move(newCard));
}

void Player::playCard(int handIndex, Board& board, EventManager& eventManager,Position target) {
    if (d_hasPlayedCardThisTurn) return; 
    if (handIndex < 0 || handIndex >= d_hand.size()) return;
    d_hand[handIndex]->play(*this, board, eventManager,target);

    
    d_hand.erase(d_hand.begin() + handIndex);
    d_hasPlayedCardThisTurn = true;
}

bool Player::hasPlayedCardThisTurn() const { 
    return d_hasPlayedCardThisTurn; 
}

void Player::resetTurn() { 
    d_hasPlayedCardThisTurn = false; 
}
void Player::addScore(int points) { d_score += points; }
int Player::getScore() const { return d_score; }
void Player::spendScore(int amount) { d_score -= amount; }