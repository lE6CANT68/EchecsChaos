#pragma once
#include "Card.h"
#include "../Core/Event/EventList/MeteoriteEvent.h"

class MeteoriteCard : public Card {
public:
    std::string getName() const override { return "Meteorite"; }
    std::string getDescription() const override { return "Cible une case au hasard. Détruit tout au prochain tour (sauf le Roi)."; }
    CardRarity getRarity() const override { return CardRarity::Rare; }
    int getCost() const override { return 4; } 

    void play(Player& player ,Board& board, EventManager& eventManager,Position target) override {

        auto newMeteorite = std::make_unique<MeteoriteEvent>(eventManager.getAudio());
        
        eventManager.addEvent(board, std::move(newMeteorite));
    }
};