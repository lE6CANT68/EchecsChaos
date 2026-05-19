#include "Idiot.h"
#include "../Base/Board.h"

// On s'assure de bien lui donner le type PieceType::Idiot (à rajouter dans ton enum si ce n'est pas fait !)
Idiot::Idiot(PieceColor c) : Piece(c, PieceType::Idiot) {}

std::vector<Position> Idiot::getValidMoves(Position currentPos, const Board& board) const {
    std::vector<Position> moves;
    
    moves.reserve(MAX_MOVES);

    std::vector<Position> directions = {
        {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1},  {0, 1},  {1, 1}  
    };

    for (const Position& dir : directions) {
        Position targetPos = { currentPos.x + dir.x, currentPos.y + dir.y };

        if (board.isinBounds(targetPos)) {
            
            const Tile& tile = board.getTile(targetPos);
            
            if (!tile.isWalkable()) {
                continue; 
            }
            
            // L'Idiot peut capturer :
            // - Des pièces de couleur différente (sauf King)
            // - Le Duck même s'il est Neutral (pour la capture mutuelle)
            if (!tile.hasPiece() || 
                (tile.getPiece()->getColor() != this->getColor() && tile.getPiece()->getType() != PieceType::King) ||
                (this->getColor() == PieceColor::Neutral && tile.getPiece()->getType() == PieceType::Duck)) {
                moves.push_back(targetPos);
            }
        }
    }

    return moves;
}