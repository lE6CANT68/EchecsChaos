#include "Pawn.h"
#include "../Base/Board.h" 

Pawn::Pawn(PieceColor c):Piece(c,PieceType::Pawn){}


std::vector<Position> Pawn::getValidMoves(Position currentPos, const Board& board) const {
    std::vector<Position> moves;
    moves.reserve(MAX_MOVES);

    int direction = (getColor() == PieceColor::White) ? -1 : 1;

    //Forward move
    Position forward = { currentPos.x, currentPos.y + direction };
    if (board.isinBounds(forward) && !board.getTile(forward).hasPiece()) {
        moves.push_back(forward);
    }
    //Eat in diag left
    Position diagLeft = { currentPos.x - 1, currentPos.y + direction };
    if (board.isinBounds(diagLeft) && board.getTile(diagLeft).hasPiece()) {
        if (board.getTile(diagLeft).getPiece()->getColor() != this->getColor()) {
            moves.push_back(diagLeft);
        }
    }
    //Eat in diag right
    Position diagRight = { currentPos.x + 1, currentPos.y + direction };
    if (board.isinBounds(diagRight) && board.getTile(diagRight).hasPiece()) {
        if (board.getTile(diagRight).getPiece()->getColor() != this->getColor()) {
            moves.push_back(diagRight);
        }
    }

    //Double Move at the start

    if (!this->hasMoved()) {
        Position doubleForward = { currentPos.x, currentPos.y + (2 * direction) };
        
        if (board.isinBounds(doubleForward) && 
            !board.getTile(forward).hasPiece() && 
            !board.getTile(doubleForward).hasPiece()) {
            
            moves.push_back(doubleForward);
        }
    }

    //En passant
    Position enPassantTarget = board.getEnPassantTarget();
    if (enPassantTarget.x != -1 && enPassantTarget.y != -1) {
        
        if ((currentPos.x - 1 == enPassantTarget.x || currentPos.x + 1 == enPassantTarget.x) &&
            (currentPos.y + direction == enPassantTarget.y)) {
            
            moves.push_back(enPassantTarget); 
        }
    }
    return moves;
}