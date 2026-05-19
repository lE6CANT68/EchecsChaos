#pragma once
#include "Card.h"

class TeleportCard : public Card {
public:
    std::string getName() const override { return "Teleportation"; }
    std::string getDescription() const override { return "Teleporte une piece sur une case vide (Rayon: 3). Pas de promotion possible."; }
    int getCost() const override { return 5; } 
    CardRarity getRarity() const override { return CardRarity::Epic; } 
    
    bool requiresTarget() const override { return true; } 
    bool requiresTwoTargets() const override { return true; } 

    // 1er clic : On choisit la pièce
    bool isValidTarget(Board& board, Position target) const override{
        if (!board.isinBounds(target)) return false;
        return board.getTile(target).hasPiece();
    }
    
    // 2ème clic : On choisit la destination
    bool isValidSecondTarget(Board& board, Position firstTarget, Position secondTarget) const override{
        if (!board.isinBounds(secondTarget)) return false;
        
        
        int distanceX = std::abs(secondTarget.x - firstTarget.x);
        int distanceY = std::abs(secondTarget.y - firstTarget.y);
        if (distanceX > 1|| distanceY > 1) return false;
        const Tile& destTile = board.getTile(secondTarget);
        if (destTile.hasPiece() || !destTile.isWalkable()) return false;
        Piece* piece = board.getTile(firstTarget).getPiece();
        if (piece->getType() == PieceType::Pawn) {
            if (piece->getColor() == PieceColor::White && secondTarget.y == Config::Board::PAWN_PROMOTION_WHITE_ROW) return false;
            if (piece->getColor() == PieceColor::Black && secondTarget.y == Config::Board::PAWN_PROMOTION_BLACK_ROW) return false;
        }

        return true; 
    }
    
    
    void play(Player& player, Board& board, EventManager& eventManager, Position target) override {}
    void playTwoTargets(Player& player, Board& board, EventManager& eventManager, Position firstTarget, Position secondTarget) override{
        std::unique_ptr<Piece> p = board.getTile(firstTarget).releasePiece();
        board.getTile(secondTarget).setPiece(std::move(p));

        eventManager.getAudio().playMove();
    }
};