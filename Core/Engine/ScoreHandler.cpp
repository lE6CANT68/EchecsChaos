#include "ScoreHandler.h"

int ScoreHandler::getPieceValue(PieceType type) {
    switch (type) {
        case PieceType::Pawn:   return 1;
        case PieceType::Knight: return 3;
        case PieceType::Bishop: return 3;
        case PieceType::Rook:   return 5;
        case PieceType::Queen:  return 10;
        case PieceType::King:   return 100; 
        default:                return 0;
    }
}

int ScoreHandler::calculateCaptureScore(PieceType capturedType) {
    return getPieceValue(capturedType);
}

std::vector<PieceType> ScoreHandler::getPiecesByTier(int tier) {
    if (tier == 1) return { PieceType::Pawn };
    if (tier == 2) return { PieceType::Knight, PieceType::Bishop };
    if (tier == 3) return { PieceType::Rook };
    if (tier == 4) return { PieceType::Queen };
    return {};
}