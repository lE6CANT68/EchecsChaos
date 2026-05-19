#pragma once
#include "../Base/Piece.h"

class Duck : public Piece {
public:
    Duck(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos, const Board& board) const override;
    
private:
    static constexpr int MAX_MOVES = 8;
};
