#include "Queen.h"
#include "../Base/Board.h"

Queen::Queen(PieceColor c) : Piece(c, PieceType::Queen) {}

std::vector<Position> Queen::getValidMoves(Position currentPos, const Board& board) const {
    std::vector<Position> moves;
    
    moves.reserve(MAX_MOVES);

    std::vector<Position> directions = {
        {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1},  {0, 1},  {1, 1}  
    };

        for(const Position& direction : directions){
        Position checkPos = { currentPos.x + direction.x, currentPos.y + direction.y };
        while (board.isinBounds(checkPos)) {
            
            const Tile& tile = board.getTile(checkPos);

            if (!tile.hasPiece()) {
                moves.push_back(checkPos);
            } 
            else {
                if (tile.getPiece()->getColor() != this->getColor()) {
                    moves.push_back(checkPos);
                }
                break;
            }
            checkPos.x += direction.x;
            checkPos.y += direction.y;
        }
    }

    return moves;
}