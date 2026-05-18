#include "TitleScreen.h"

TitleScreen::TitleScreen()
    : d_startButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 300, "JOUER"),
      d_settingsButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 380, "SETTINGS"),
      d_quitButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 460, "QUITTER"),
      d_selectedAction(Action::None)
{
}

void TitleScreen::update(int mouseX, int mouseY, bool isMousePressed) {
    d_startButton.update(mouseX, mouseY, false);
    d_settingsButton.update(mouseX, mouseY, false);
    d_quitButton.update(mouseX, mouseY, false);

    if (!isMousePressed) {
        return;
    }

    if (d_startButton.update(mouseX, mouseY, true)) {
        d_selectedAction = Action::Play;
        return;
    }
    if (d_settingsButton.update(mouseX, mouseY, true)) {
        d_selectedAction = Action::Settings;
        return;
    }
    if (d_quitButton.update(mouseX, mouseY, true)) {
        d_selectedAction = Action::Quit;
        return;
    }
}

void TitleScreen::draw() const {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    const char* titleText = "Echecs Chaos";
    const int titleSize = 60;
    int titleWidth = MeasureText(titleText, titleSize);

    DrawText(titleText, (Config::Graphics::CONFIG_WINDOW_WIDTH - titleWidth) / 2, 140, titleSize, RAYWHITE);
    DrawText("CECI EST UN PLACEHOLDER", 260, 220, 24, LIGHTGRAY);

    d_startButton.draw();
    d_settingsButton.draw();
    d_quitButton.draw();
    EndDrawing();
}

TitleScreen::Action TitleScreen::getSelectedAction() const {
    return d_selectedAction;
}

void TitleScreen::clearAction() {
    d_selectedAction = Action::None;
}
