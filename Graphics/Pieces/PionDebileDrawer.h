#pragma once
#include "PieceDrawer.h"

class PionDebileDrawer : public PieceDrawer {
public:
    PionDebileDrawer() : PieceDrawer("", "") {}

    void draw(float centerX, float centerY, Color pieceColor, float cellSize) const override {
        // Dessiner un cercle avec un "D" au centre
        float radius = (cellSize * 0.4f);
        
        // Dessiner le cercle avec la couleur de la pièce
        DrawCircle((int)centerX, (int)centerY, radius, pieceColor);
        
        // Ajouter la bordure noire (fonction standard Raylib)
        DrawCircleLines((int)centerX, (int)centerY, radius, BLACK);
        
        // Dessiner le "D" au centre
        const char* text = "D";
        int fontSize = (int)(cellSize * 0.5f);
        int textWidth = MeasureText(text, fontSize);
        
        // Si la couleur de la pièce est claire, le texte sera NOIR.
        // Sinon (comme BLACK ou gris foncé), le texte sera BLANC.
        Color textColor = (pieceColor.r > 128) ? BLACK : WHITE;
        
        DrawText(text, 
                 (int)(centerX - textWidth / 2.0f), 
                 (int)(centerY - fontSize / 2.0f), 
                 fontSize, 
                 textColor);
    }
};