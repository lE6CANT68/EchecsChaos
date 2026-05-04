#pragma once
#include "PieceDrawer.h"
#include "../Core/Config/PiecesPath.h"
#include <algorithm>


class KingDrawer : public PieceDrawer {
public:
    KingDrawer():PieceDrawer(PiecePaths::KING_WHITE, PiecePaths::KING_BLACK) { }
};