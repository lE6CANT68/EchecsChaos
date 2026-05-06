#pragma once
#include "../Base/Board.h"
#include "../Base/Position.h"
#include "../Audio/AudioManager.h"
#include <vector>

class BoardInteractionManager {
public:
    static bool tryExecuteMove(Board& board, Position clickedPos, Position& selectedTile, const std::vector<Position>& validMoves, AudioManager& audioManager);

    static void updateSelection(Board& board, Position clickedPos, PieceColor currentColor, Position& selectedTile, std::vector<Position>& validMoves);
};