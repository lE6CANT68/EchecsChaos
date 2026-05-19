#pragma once
#include "../Base/Board.h"
#include "../Base/Position.h"
#include "../Config/Constant.h"

class SpecialMoveHandler {
public:
    static void handleCastling(Board& board, Position startPos, Position targetPos);
    static void handleEnPassant(Board& board, Position startPos, Position targetPos);
    static void updateEnPassantTarget(Board& board, Position startPos, Position targetPos);
};