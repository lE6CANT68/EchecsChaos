#pragma once
#include "PieceDrawer.h"
#include "../Core/Config/PiecesPath.h"

class ChameleonDrawer : public PieceDrawer {
public:
    ChameleonDrawer():PieceDrawer(PiecePaths::CHAMELEON_WHITE, PiecePaths::CHAMELEON_BLACK) {}
};
