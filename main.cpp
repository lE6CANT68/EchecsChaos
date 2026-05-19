#include "raylib.h"
#include "../Graphics/UI/Settings.h"
#include "../Core/Engine/GameEngine.h"
#include "../Core/Config/Constant.h"

int main() {
    // Initialisation temporaire pour charger les paramètres
    Settings tempSettings;
    tempSettings.load();

    InitWindow(tempSettings.getWindowWidth(), tempSettings.getWindowHeight(), "Echecs Chaos - v0.2");
    InitAudioDevice();
    SetTargetFPS(Config::Graphics::CONFIG_TARGET_FPS);

    {
        GameEngine engine;
        engine.run();
    }

    CloseWindow();
    return 0;
}