#include "raylib.h"
#include "../Core/Engine/GameEngine.h"

int main() {
    InitWindow(800, 800, "Echecs Chaos - V0.1");
    InitAudioDevice();
    SetTargetFPS(60);
    {
        GameEngine engine; 
        engine.run(); 
    } 

    CloseWindow();
    return 0;
}