#pragma once
#include "../Base/Piece.h"
#include <vector>
#include <map>

class ScoreHandler {
public:
    static int getPieceValue(PieceType type);

    static std::vector<PieceType> getPiecesByTier(int tier);
    
    static int calculateCaptureScore(PieceType capturedType);
};