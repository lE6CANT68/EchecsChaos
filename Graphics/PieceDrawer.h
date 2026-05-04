#pragma once
#include "raylib.h"
#include <algorithm>

class PieceDrawer {
public:
    PieceDrawer(const char* whitePath, const char* blackPath) {
        d_whiteTexture = LoadTexture(whitePath);
        d_blackTexture = LoadTexture(blackPath);

        GenTextureMipmaps(&d_whiteTexture);
        SetTextureFilter(d_whiteTexture, TEXTURE_FILTER_BILINEAR);
        GenTextureMipmaps(&d_blackTexture);
        SetTextureFilter(d_blackTexture, TEXTURE_FILTER_BILINEAR);
    }

    virtual ~PieceDrawer() {
        if (d_whiteTexture.id != 0) UnloadTexture(d_whiteTexture);
        if (d_blackTexture.id != 0) UnloadTexture(d_blackTexture);
    }
    virtual void draw(float centerX, float centerY, Color pieceColor, float cellSize) const {
        Texture2D texToDraw = (pieceColor.r == 255) ? d_whiteTexture : d_blackTexture;

        if (texToDraw.id != 0) {
            float pieceSizeFactor = 0.85f;
            float maxImageDim = (float)std::max(texToDraw.width, texToDraw.height);
            float scale = (cellSize * pieceSizeFactor) / maxImageDim;
            float actualWidth = texToDraw.width * scale;
            float actualHeight = texToDraw.height * scale;

            float offsetX = 2.0f;  //offset du cul
            float offsetY = -0.0f;
            
            Vector2 pos = { 
                centerX - (actualWidth / 2.0f) + offsetX, 
                centerY - (actualHeight / 2.0f) + offsetY
            };

            DrawTextureEx(texToDraw, pos, 0.0f, scale, WHITE);
        }
    }

protected:
    Texture2D d_whiteTexture;
    Texture2D d_blackTexture;
};