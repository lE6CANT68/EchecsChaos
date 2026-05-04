#pragma once
#include "PieceDrawer.h"
#include "../Core/Config/PiecesPath.h"
#include <algorithm>

class BishopDrawer : public PieceDrawer {
public:
    BishopDrawer():PieceDrawer(PiecePaths::BISHOP_WHITE, PiecePaths::BISHOP_BLACK){}
};