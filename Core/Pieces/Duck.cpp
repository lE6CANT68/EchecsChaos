#include "Duck.h"
#include "../Base/Board.h"
#include "raylib.h"

Duck::Duck(PieceColor c) : Piece(c, PieceType::Duck) {}

std::vector<Position> Duck::getValidMoves(Position currentPos, const Board& board) const {
    std::vector<Position> moves;
    moves.reserve(MAX_MOVES);

    // Le canard se déplace comme un cavalier (knight)
    std::vector<Position> knightMoves = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };

    for (const Position& move : knightMoves) {
        Position targetPos = { currentPos.x + move.x, currentPos.y + move.y };

        if (board.isinBounds(targetPos)) {
            const Tile& tile = board.getTile(targetPos);
            
            if (!tile.isWalkable()) {
                continue;
            }
            
            // Le canard peut capturer :
            // - Des pièces de couleur différente
            // - L'Idiot même s'il est Neutral (pour la capture mutuelle)
            if (!tile.hasPiece() || 
                tile.getPiece()->getColor() != this->getColor() ||
                (this->getColor() == PieceColor::Neutral && tile.getPiece()->getType() == PieceType::Idiot)) {
                moves.push_back(targetPos);
            }
        }
    }

    return moves;
}
