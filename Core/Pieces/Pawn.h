#pragma once
#include "../Base/Piece.h"

class Pawn : public Piece{
public:
    Pawn(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos,const Board& board)const;
private:
    static constexpr int MAX_MOVES = 13;
};