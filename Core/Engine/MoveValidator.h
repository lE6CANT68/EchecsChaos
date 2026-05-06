#pragma once
#include <vector>
#include "../Base/Board.h"
#include "../Base/Position.h"

class MoveValidator {
public:
    static std::vector<Position> filterLegalMoves(Board& board, Position startPos, const std::vector<Position>& pseudoMoves);
};