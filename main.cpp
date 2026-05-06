#include "raylib.h"
#include "../Core/Engine/GameEngine.h"
#include "../Core/Config/Constant.h"

int main() {
    InitWindow(Config::Graphics::CONFIG_WINDOW_WIDTH, Config::Graphics::CONFIG_WINDOW_HEIGHT, "Echecs Chaos - v0.2");
    InitAudioDevice();
    SetTargetFPS(Config::Graphics::CONFIG_TARGET_FPS);
    {
        GameEngine engine; 
        engine.run(); 
    } 

    CloseWindow();
    return 0;
}