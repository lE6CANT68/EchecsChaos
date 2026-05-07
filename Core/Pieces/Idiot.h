#pragma once
#include "../Base/Piece.h"

class Idiot : public Piece {
public:
    Idiot(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos, const Board& board) const override;
    
private:
    static constexpr int MAX_MOVES = 8;
};