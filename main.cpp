#include "raylib.h"
#include "../Core/Engine/GameEngine.h"

int main() {
    InitWindow(1000, 1000, "Echecs Chaos - v0.2");
    InitAudioDevice();
    SetTargetFPS(60);
    {
        GameEngine engine; 
        engine.run(); 
    } 

    CloseWindow();
    return 0;
}