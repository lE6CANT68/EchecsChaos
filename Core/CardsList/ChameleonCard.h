#pragma once
#include "Card.h"
#include "../Base/Player.h"
#include "../Core/Event/EventList/FogEvent.h"
#include "../Pieces/Chameleon.h"
#include "../Engine/ScoreHandler.h"

class FogCard : public Card {
public:
    std::string getName() const override { return "Bombe Fumigene"; }
    std::string getDescription() const override { return "Cree un brouillard 3x3 obscurcissant les pieces pour 4 tours."; }
    int getCost() const override { return 3; }
    CardRarity getRarity() const override { return CardRarity::Legendary; }

    bool requiresTarget() const override { return true; }
    

    void play(Player& player, Board& board, EventManager& eventManager, Position target) {
    PieceColor color = board.getTile(target).getPiece()->getColor();
    board.getTile(target).setPiece(std::make_unique<Chameleon>(color));
    
    eventManager.getAudio().playMove(); 
}
    bool isValidTarget(Board& board, Position target) const {
    if (!board.isinBounds(target)) return false;
    const Tile& tile = board.getTile(target);
    
    if (!tile.hasPiece()) return false;
    PieceType targetType = tile.getPiece()->getType();
    return ScoreHandler::getPieceValue(targetType) == 3;
}
};




