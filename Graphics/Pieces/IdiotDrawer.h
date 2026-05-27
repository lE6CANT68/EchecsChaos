#pragma once
#include "PieceDrawer.h"
#include "raylib.h"
#include <algorithm>

class IdiotDrawer : public PieceDrawer {
private:
    Texture2D d_idiotTexture;

public:
    IdiotDrawer() : PieceDrawer("", "") {
        d_idiotTexture = LoadTexture("../assets/Pieces/idiot.png");
        GenTextureMipmaps(&d_idiotTexture);
        SetTextureFilter(d_idiotTexture, TEXTURE_FILTER_BILINEAR);
    }
    
    ~IdiotDrawer() override {
        if (d_idiotTexture.id != 0) UnloadTexture(d_idiotTexture);
    }
    
    void draw(float centerX, float centerY, Color pieceColor, float cellSize) const override {
        if (d_idiotTexture.id != 0) {
            float pieceSizeFactor = 0.85f;
            float maxImageDim = (float)std::max(d_idiotTexture.width, d_idiotTexture.height);
            float scale = (cellSize * pieceSizeFactor) / maxImageDim;
            float actualWidth = d_idiotTexture.width * scale;
            float actualHeight = d_idiotTexture.height * scale;

            float offsetX = 2.0f;
            float offsetY = -0.0f;
            
            Vector2 pos = { 
                centerX - (actualWidth / 2.0f) + offsetX, 
                centerY - (actualHeight / 2.0f) + offsetY
            };

            DrawTextureEx(d_idiotTexture, pos, 0.0f, scale, WHITE);
        }
    }

};