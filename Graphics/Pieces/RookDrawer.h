#pragma once
#include "PieceDrawer.h"
#include "../Core/Config/PiecesPath.h"
#include <algorithm>

class RookDrawer : public PieceDrawer {
public:
    RookDrawer( ) : PieceDrawer(PiecePaths::ROOK_WHITE, PiecePaths::ROOK_BLACK){};
};