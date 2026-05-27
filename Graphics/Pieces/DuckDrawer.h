#pragma once
#include "PieceDrawer.h"
#include "raylib.h"
#include <algorithm>

class DuckDrawer : public PieceDrawer {
private:
    Texture2D d_duckTexture;

public:
    DuckDrawer() : PieceDrawer("", "") {
        d_duckTexture = LoadTexture("../assets/Pieces/duck.png");
        GenTextureMipmaps(&d_duckTexture);
        SetTextureFilter(d_duckTexture, TEXTURE_FILTER_BILINEAR);
    }
    
    ~DuckDrawer() override {
        if (d_duckTexture.id != 0) UnloadTexture(d_duckTexture);
    }
    
    void draw(float centerX, float centerY, Color pieceColor, float cellSize) const override {
        if (d_duckTexture.id != 0) {
            float pieceSizeFactor = 0.85f;
            float maxImageDim = (float)std::max(d_duckTexture.width, d_duckTexture.height);
            float scale = (cellSize * pieceSizeFactor) / maxImageDim;
            float actualWidth = d_duckTexture.width * scale;
            float actualHeight = d_duckTexture.height * scale;

            float offsetX = 2.0f;
            float offsetY = -0.0f;
            
            Vector2 pos = { 
                centerX - (actualWidth / 2.0f) + offsetX, 
                centerY - (actualHeight / 2.0f) + offsetY
            };

            DrawTextureEx(d_duckTexture, pos, 0.0f, scale, WHITE);
        }
    }

};
