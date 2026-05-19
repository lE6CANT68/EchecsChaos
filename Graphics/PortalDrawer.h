#pragma once
#include "raylib.h"
#include <cmath>

class PortalDrawer {
public:
    /**
     * @brief Dessine un portail animé
     * @param centerX Position X du centre de la case
     * @param centerY Position Y du centre de la case
     * @param cellSize Taille de la case pour l'adaptation
     */
    static void draw(float centerX, float centerY, float cellSize) {
        float time = (float)GetTime();
        
        // 1. Effet de pulsation (taille qui varie légèrement)
        float pulse = 0.9f + 0.1f * sinf(time * 4.0f);
        float baseRadius = (cellSize * 0.35f) * pulse;

        // 2. Dessin du halo extérieur (Violet néon)
        Color portalColor = { 150, 0, 255, 255 }; 
        DrawCircleGradient((int)centerX, (int)centerY, baseRadius * 1.2f, portalColor, BLANK);

        // 3. Dessin des anneaux rotatifs
        for (int i = 0; i < 3; i++) {
            float rotation = time * (2.0f + i);
            float radius = baseRadius - (i * 5.0f);
            
            // On dessine des arcs pour simuler un vortex en rotation
            DrawRing(
                { centerX, centerY },
                radius - 2.0f,
                radius,
                rotation * 50.0f,
                rotation * 50.0f + 270.0f,
                20,
                WHITE
            );
        }

        // 4. Centre sombre (le "trou noir")
        DrawCircle((int)centerX, (int)centerY, baseRadius * 0.4f, BLACK);
    }
};