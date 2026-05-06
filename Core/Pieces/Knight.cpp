#include "Knight.h"
#include "../Base/Board.h"
Knight::Knight(PieceColor c) : Piece(c, PieceType::Knight) {}

std::vector<Position> Knight::getValidMoves(Position currentPos, const Board& board) const {
    std::vector<Position> moves;

    moves.reserve(MAX_MOVES);
    std::vector<Position> jumps = {
        {-2, -1}, {-2, 1}, 
        {-1, -2}, {-1, 2},
        {1, -2},  {1, 2}, 
        {2, -1},  {2, 1}
    };

    for (const Position& jump : jumps) {
        Position targetPos = { currentPos.x + jump.x, currentPos.y + jump.y };
        if (board.isinBounds(targetPos)) {
            
            const Tile& tile = board.getTile(targetPos);
            if (!tile.isWalkable()) {
                break; 
            }
            if (!tile.hasPiece() || tile.getPiece()->getColor() != this->getColor()) {
                moves.push_back(targetPos);
            }
        }
    }

    return moves;
}