#pragma once
#include "../Base/Piece.h"

class King : public Piece{
public:
    King(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos,const Board& board)const;
private:
    static constexpr int MAX_MOVES = 8;
};