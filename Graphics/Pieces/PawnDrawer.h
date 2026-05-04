#pragma once
#include "PieceDrawer.h"
#include "../Core/Config/PiecesPath.h"
#include <algorithm> 

class PawnDrawer : public PieceDrawer {
public:
    PawnDrawer():PieceDrawer(PiecePaths::PAWN_WHITE, PiecePaths::PAWN_BLACK) {}

};