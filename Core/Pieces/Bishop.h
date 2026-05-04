#pragma once
#include "../Base/Piece.h"

class Bishop : public Piece{
public:
    Bishop(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos,const Board& board)const;
private:
    static constexpr int MAX_MOVES = 13;
};