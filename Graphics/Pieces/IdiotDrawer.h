#pragma once
#include "PieceDrawer.h"
#include "../Core/Config/PiecesPath.h"
#include "raylib.h"

class IdiotDrawer : public PieceDrawer {
public:

    IdiotDrawer() : PieceDrawer(PiecePaths::PAWN_WHITE, PiecePaths::PAWN_BLACK) {}


    void draw(float centerX, float centerY, Color pieceColor, float cellSize) const override 
    {

        float radius = (cellSize / 2.0f) * 0.8f; 
        DrawCircle((int)centerX, (int)centerY, radius, PURPLE);


        DrawCircleLines((int)centerX, (int)centerY, radius, YELLOW);

        const char* text = "I";
        int fontSize = (int)(cellSize * 0.7f); 
        int textWidth = MeasureText(text, fontSize);
        
        int textX = (int)(centerX - (textWidth / 2.0f));
        int textY = (int)(centerY - (fontSize / 2.0f));

        DrawText(text, textX, textY, fontSize, YELLOW);
    }
};