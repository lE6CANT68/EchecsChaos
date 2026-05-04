#pragma once
#include "PieceDrawer.h"
#include "../Core/Config/PiecesPath.h"
#include <algorithm>

class KnightDrawer : public PieceDrawer {
public:
    KnightDrawer():PieceDrawer(PiecePaths::KNIGHT_WHITE, PiecePaths::KNIGHT_BLACK) {}
    
};