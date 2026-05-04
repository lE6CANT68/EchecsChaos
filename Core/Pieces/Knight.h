#pragma once
#include "../Base/Piece.h"

class Knight : public Piece {
public:
    Knight(PieceColor c);

    std::vector<Position> getValidMoves(Position currentPos, const Board& board) const override;
private:
    static constexpr int MAX_MOVES = 8;
};