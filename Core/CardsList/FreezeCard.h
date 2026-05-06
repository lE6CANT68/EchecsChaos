#pragma once
#include "Card.h"
#include "../Base/Player.h"
#include "../Core/Event/EventList/FreezeEvent.h"

class FreezeCard : public Card {
public:
    std::string getName() const override { return "Statue de Glace"; }
    std::string getDescription() const override { return "Gele une piece (sauf Roi) pour 3 tours."; }
    int getCost() const override { return 2; }
    CardRarity getRarity() const override { return CardRarity::Common; }

    bool requiresTarget() const override { return true; }

    void play(Player& player, Board& board, EventManager& eventManager, Position target) override {
        // On s'assure juste que le clic est valide avant de créer l'événement
        if (board.isinBounds(target)) {
            Tile& tile = board.getTile(target);
            if (tile.hasPiece() && tile.getPiece()->getType() != PieceType::King) {
                
                auto freezeEvent = std::make_unique<FreezeEvent>(target, 3, eventManager.getAudio());
                eventManager.addEvent(board, std::move(freezeEvent));
            }
        }
    }
};