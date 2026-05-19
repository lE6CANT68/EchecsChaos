#pragma once
#include <string>
#include <memory>
#include "../Core/Event/EventManager.h"
#include "../Core/Base/Board.h"

#pragma once
class Player; 
class Board;
class EventManager;

enum class CardRarity {
    Common,     // Gris 
    Rare,       // Bleu 
    Epic,       // Violet 
    Legendary   // Doré 
};

class Card {
public:
    virtual ~Card() = default;

    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual CardRarity getRarity() const = 0;
    virtual int getCost() const = 0; 
    virtual bool requiresTarget() const { return false; }
    virtual bool isValidTarget(Board& board, Position target) const { return true; }

    virtual void play(Player& player, Board& board, EventManager& eventManager, Position target = Position::NONE) = 0;

    virtual bool requiresTwoTargets() const { return false; } 
    
    virtual bool isValidSecondTarget(Board& board, Position firstTarget, Position secondTarget) const { return false; }
    
    virtual void playTwoTargets(Player& player, Board& board, EventManager& eventManager, Position firstTarget, Position secondTarget) {}
};