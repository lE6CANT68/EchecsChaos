#pragma once
#include "../Base/Piece.h"

class Queen : public Piece{
public:
    Queen(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos,const Board& board)const;
private:
    static constexpr int MAX_MOVES = 27;
};