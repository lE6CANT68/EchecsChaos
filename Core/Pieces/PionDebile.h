#pragma once
#include "../Base/Piece.h"

class PionDebile : public Piece {
public:
    PionDebile(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos, const Board& board) const override;
    
private:
    static constexpr int MAX_MOVES = 3; 
};