#pragma once
#include "PieceDrawer.h"
#include "../Core/Config/PiecesPath.h"
#include <algorithm>

class QueenDrawer : public PieceDrawer {
public:
    QueenDrawer() :PieceDrawer(PiecePaths::QUEEN_WHITE, PiecePaths::QUEEN_BLACK){}


};