#pragma once
#include "Card.h"
#include "../Base/Player.h"
#include "../Core/Event/EventList/LavaWallEvent.h" // Adapte le chemin vers ton dossier d'événements

class LavaWallCard : public Card {
public:
    std::string getName() const override { return "Mur de Lave"; }
    std::string getDescription() const override { return "Cree un mur de lave horizontal (3 cases). Fait fondre les pieces et bloque le passage pour 4 tours."; }
    int getCost() const override { return 10; } 
    CardRarity getRarity() const override { return CardRarity::Legendary; } 
    
    bool requiresTarget() const override { return true; } 

    void play(Player& player, Board& board, EventManager& eventManager, Position target) override{
        if (!board.isinBounds(target)) return;
        std::vector<Position> wallPos = {
            {target.x - 1, target.y},
            target,
            {target.x + 1, target.y}
        };
        auto lavaEvent = std::make_unique<LavaWallEvent>(wallPos, 6, eventManager.getAudio());
        eventManager.addEvent(board, std::move(lavaEvent));
    }
    bool isValidTarget(Board& board, Position target) const override{
        if (!board.isinBounds(target)) return false;

        std::vector<Position> wallPos = {
            {target.x - 1, target.y},
            target,
            {target.x + 1, target.y}
        };
        for (const Position& pos : wallPos) {
            if (board.isinBounds(pos)) {
                if (board.getTile(pos).hasPiece() || !board.getTile(pos).isWalkable()) {
                    return false;
                }
            }
        }
        return true; 
    }
};