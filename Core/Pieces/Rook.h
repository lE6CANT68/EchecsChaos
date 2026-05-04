#pragma once
#include "../Base/Piece.h"

class Rook : public Piece{
public:
    Rook(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos,const Board& board)const;
private:
    static constexpr int MAX_MOVES = 14;
};