#pragma once
#include "PieceDrawer.h"

class DuckDrawer : public PieceDrawer {
public:
    DuckDrawer() : PieceDrawer("", "") {}
    
    void draw(float centerX, float centerY, Color pieceColor, float cellSize) const override {
        // Couleur orange pour le canard (ignore pieceColor)
        Color duckColor = ORANGE;

        // Dessin du cercle de bordure
        float radius = (cellSize / 2.0f) * 0.7f;
        DrawCircleLines((int)centerX, (int)centerY, (int)radius, duckColor);
        
        // Remplissage léger
        DrawCircle((int)centerX, (int)centerY, radius - 2.0f, Fade(duckColor, 0.3f));

        // Dessin du "D" au centre
        const char* text = "D";
        
        int fontSize = (int)(cellSize * 0.6f);
        int textWidth = MeasureText(text, fontSize);
        
        DrawText(text, 
                 (int)(centerX - textWidth / 2), 
                 (int)(centerY - fontSize / 2), 
                 fontSize, 
                 duckColor);
    }
};
