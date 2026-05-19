#pragma once
#include "Card.h"
#include "../Base/Player.h"
#include "../Core/Event/EventList/FogEvent.h"

class FogCard : public Card {
public:
    std::string getName() const override { return "Bombe Fumigene"; }
    std::string getDescription() const override { return "Cree un brouillard 3x3 obscurcissant les pieces pour 4 tours."; }
    int getCost() const override { return 3; }
    CardRarity getRarity() const override { return CardRarity::Rare; }

    bool requiresTarget() const override { return true; }
    

    void play(Player& player, Board& board, EventManager& eventManager, Position target) override {
        auto fogEvent = std::make_unique<FogEvent>(target, 4, eventManager.getAudio());
        eventManager.addEvent(board, std::move(fogEvent));
    }
};