#include "Chameleon.h"
#include "../Base/Board.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"

Chameleon::Chameleon(PieceColor c) : Piece(c, PieceType::Chameleon) {}

std::vector<Position> Chameleon::getValidMoves(Position currentPos, const Board& board) const {
    PieceType mimicType = board.getLastMovedPieceType();
    if (mimicType == PieceType::Chameleon) mimicType = PieceType::Queen; 
    switch (mimicType) {
        case PieceType::Pawn:   return Pawn(getColor()).getValidMoves(currentPos, board);
        case PieceType::Knight: return Knight(getColor()).getValidMoves(currentPos, board);
        case PieceType::Bishop: return Bishop(getColor()).getValidMoves(currentPos, board);
        case PieceType::Rook:   return Rook(getColor()).getValidMoves(currentPos, board);
        case PieceType::Queen:  return Queen(getColor()).getValidMoves(currentPos, board);
        case PieceType::King:   return King(getColor()).getValidMoves(currentPos, board);
        default:                return {};
    }
}