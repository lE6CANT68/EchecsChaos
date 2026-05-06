#pragma once
#include "Card.h"
#include "../Base/Player.h"
#include "../Core/Event/EventList/HideTimeEvent.h"

class HideTimeCard : public Card {
public:
    std::string getName() const override { return "Sabotage Temporel"; }
    std::string getDescription() const override { return "Masque l'affichage du temps pour les 3 prochains tours."; }
    int getCost() const override { return 4; } 
    CardRarity getRarity() const override { return CardRarity::Epic; }
    bool requiresTarget() const override { return false; } 

    void play(Player& player, Board& board, EventManager& eventManager, Position target) override {
        PlayerTarget victim = (player.getColor() == PieceColor::White) ? PlayerTarget::Black : PlayerTarget::White;
        auto hideEvent = std::make_unique<HideTimeEvent>(victim, 3, eventManager.getAudio());
        eventManager.addEvent(board, std::move(hideEvent));
    }
};