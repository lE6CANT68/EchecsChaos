#include "King.h"
#include "../Base/Board.h"

King::King(PieceColor c) : Piece(c, PieceType::King) {}

std::vector<Position> King::getValidMoves(Position currentPos, const Board& board) const {
    std::vector<Position> moves;
    
    moves.reserve(MAX_MOVES);

    std::vector<Position> directions = {
        {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1},  {0, 1},  {1, 1}  
    };

    for (const Position& dir : directions) {
        Position targetPos = { currentPos.x + dir.x, currentPos.y + dir.y };

        if (board.isinBounds(targetPos)) {
            
            const Tile& tile = board.getTile(targetPos);
            
            if (!tile.hasPiece() || tile.getPiece()->getColor() != this->getColor()) {
                moves.push_back(targetPos);
            }
        }
    }

    if (!this->hasMoved()) {
        int y = currentPos.y; 
        //Roque
        if (board.isinBounds({5, y}) && !board.getTile({5, y}).hasPiece() && 
            board.isinBounds({6, y}) && !board.getTile({6, y}).hasPiece()) {

            if (board.getTile({7, y}).hasPiece()) {
                Piece* rightRook = board.getTile({7, y}).getPiece();
                
                if (rightRook->getType() == PieceType::Rook && 
                    rightRook->getColor() == this->getColor() && 
                    !rightRook->hasMoved()) {
                    
                    moves.push_back({6, y}); 
                }
            }
        }
        //Big Roque
        if (board.isinBounds({1, y}) && !board.getTile({1, y}).hasPiece() && 
            board.isinBounds({2, y}) && !board.getTile({2, y}).hasPiece() && 
            board.isinBounds({3, y}) && !board.getTile({3, y}).hasPiece()) {
            
            
            if (board.getTile({0, y}).hasPiece()) {
                Piece* leftRook = board.getTile({0, y}).getPiece();
                if (leftRook->getType() == PieceType::Rook && 
                    leftRook->getColor() == this->getColor() && 
                    !leftRook->hasMoved()) {
                    
                    moves.push_back({2, y}); 
                }
            }
        }
    }


    return moves;
}