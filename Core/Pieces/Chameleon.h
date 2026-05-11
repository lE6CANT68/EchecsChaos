#pragma once
#include "../Base/Piece.h"

class Chameleon : public Piece {
public:
    Chameleon(PieceColor c);
    std::vector<Position> getValidMoves(Position currentPos, const Board& board) const override;
};