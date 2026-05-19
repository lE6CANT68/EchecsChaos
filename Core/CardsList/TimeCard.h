#pragma once
#include "Card.h"
#include "../Base/Player.h"
#include "../Core/Event/EventList/TimeEvent.h"

class TimeCard : public Card {
public:
    std::string getName() const override { return "Sablier Magique"; }
    std::string getDescription() const override { return "Ajoute 30s a votre chrono."; }
    int getCost() const override { return 2; }
    CardRarity getRarity() const override { return CardRarity::Common; }

    void play(Player& player, Board& board, EventManager& eventManager,Position target) override {
        auto timeEvent = std::make_unique<TimeEvent>(player, 30.0f, eventManager.getAudio());
        eventManager.addEvent(board, std::move(timeEvent));
    }
};