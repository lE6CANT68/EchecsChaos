#pragma once
#include "Card.h"
#include "../Base/Player.h"
#include "../Core/Event/EventList/ObstacleEvent.h"

class BlockCard : public Card {
public:
    std::string getName() const override { return "Chainblock"; }
    std::string getDescription() const override { return "Bloque une case vide au hasard pour 4 tours."; }
    int getCost() const override { return 3; }
    CardRarity getRarity() const override { return CardRarity::Rare; }
    bool requiresTarget() const override { return true; }

void play(Player& player, Board& board, EventManager& eventManager, Position target) override {
        // On n'a plus besoin de chercher au hasard, on utilise 'target' direct !
        if (target.x != -1 && target.y != -1 ) {
            auto obstacleEvent = std::make_unique<ObstacleEvent>(target, 4, eventManager.getAudio());
            eventManager.addEvent(board, std::move(obstacleEvent));
        }
    }
};