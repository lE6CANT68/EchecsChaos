#pragma once
#include "Card.h"
#include "../Base/Player.h"
#include "../Core/Event/EventList/PawnSprintEvent.h"

class PawnSprintCard : public Card {
public:
    std::string getName() const override { return "Sprint du Pion"; }
    std::string getDescription() const override { return "Permet a un pion d'avancer de 2 cases (."; }
    int getCost() const override { return 1; }
    CardRarity getRarity() const override { return CardRarity::Common; }

    bool requiresTarget() const override { return true; }

    void play(Player& player, Board& board, EventManager& eventManager, Position target) override {
        if (!board.isinBounds(target)) return;

        Tile& tile = board.getTile(target);
        
        if (tile.hasPiece()) {
            Piece* piece = tile.getPiece();
            if (piece->getType() == PieceType::Pawn && piece->getColor() == player.getColor()) {
                
                auto sprintEvent = std::make_unique<PawnSprintEvent>(piece);
                eventManager.addEvent(board, std::move(sprintEvent));
                
            } else {
                // Optionnel : Message d'erreur si on clique sur un Fou ou une Reine
                // eventManager.triggerMessage("Cible invalide : Seul un pion peut sprinter !");
            }
        }
    }
};