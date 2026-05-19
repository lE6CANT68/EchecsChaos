#include "Rook.h"
#include "../Base/Board.h"

Rook::Rook(PieceColor c) : Piece(c, PieceType::Rook) {}

std::vector<Position> Rook::getValidMoves(Position currentPos, const Board& board) const {
    std::vector<Position> moves;

    moves.reserve(MAX_MOVES);

    std::vector<Position> directions = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };
    
    for (const Position& dir : directions) {
        Position checkPos = { currentPos.x + dir.x, currentPos.y + dir.y };

        while (board.isinBounds(checkPos)) {
            const Tile& tile = board.getTile(checkPos);

            if (!tile.isWalkable()) {
                break; 
            }

            if (!tile.hasPiece()) {
                moves.push_back(checkPos);
            } 
            else {
                if (tile.getPiece()->getColor() != this->getColor()) {
                    moves.push_back(checkPos);
                }
                break; 
            }

            checkPos.x += dir.x;
            checkPos.y += dir.y;
        }
    }

    return moves;
}