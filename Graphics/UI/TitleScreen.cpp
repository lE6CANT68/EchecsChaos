#include "TitleScreen.h"

TitleScreen::TitleScreen()
    : d_startButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 300, "JOUER"),
      d_settingsButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 380, "SETTINGS"),
      d_quitButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 460, "QUITTER"),
      d_selectedAction(Action::None)
{
}

void TitleScreen::setButtonClickCallback(const std::function<void()>& callback) {
    d_buttonClickCallback = callback;
    d_gamemodeMenu.setButtonClickCallback(callback);
}

void TitleScreen::update(int mouseX, int mouseY, bool isMousePressed) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Layout positions (relative to screen size)
    const int titleSize = 60;
    int titleY = (int)(screenH * 0.18f);
    int startY = (int)(screenH * 0.375f);

    float centerX = screenW / 2.0f - d_startButton.getWidth() / 2.0f;
    float spacing = d_startButton.getHeight() + 20.0f;

    d_startButton.setPosition(centerX, (float)startY);
    d_settingsButton.setPosition(centerX, (float)(startY + spacing));
    d_quitButton.setPosition(centerX, (float)(startY + 2 * spacing));

    if (d_showGamemode) {
        d_gamemodeMenu.update(mouseX, mouseY, isMousePressed);
        if (d_gamemodeMenu.getSelection() == GamemodeMenu::Selection::Local) {
            d_selectedAction = Action::Play;
            d_gamemodeMenu.clearSelection();
            d_showGamemode = false;
            return;
        }
        if (d_gamemodeMenu.getSelection() == GamemodeMenu::Selection::Back) {
            d_gamemodeMenu.clearSelection();
            d_showGamemode = false;
            return;
        }
        // AI and Online do nothing for now; keep submenu open
        return;
    }

    d_startButton.update(mouseX, mouseY, false);
    d_settingsButton.update(mouseX, mouseY, false);
    d_quitButton.update(mouseX, mouseY, false);

    if (!isMousePressed) {
        return;
    }

    if (d_startButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_showGamemode = true;
        return;
    }
    if (d_settingsButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_selectedAction = Action::Settings;
        return;
    }
    if (d_quitButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_selectedAction = Action::Quit;
        return;
    }
}

void TitleScreen::draw() const {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    const char* titleText = "Echecs Chaos";
    const int titleSize = 60;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int titleWidth = MeasureText(titleText, titleSize);

    int titleY = (int)(screenH * 0.18f);
    DrawText(titleText, (screenW - titleWidth) / 2, titleY, titleSize, RAYWHITE);

    const char* placeholder = "CECI EST UN PLACEHOLDER";
    int placeholderSize = 24;
    int placeholderWidth = MeasureText(placeholder, placeholderSize);
    int placeholderY = titleY + titleSize + 20;
    DrawText(placeholder, (screenW - placeholderWidth) / 2, placeholderY, placeholderSize, LIGHTGRAY);

    if (d_showGamemode) {
        d_gamemodeMenu.draw();
    } else {
        d_startButton.draw();
        d_settingsButton.draw();
        d_quitButton.draw();
    }
    EndDrawing();
}


TitleScreen::Action TitleScreen::getSelectedAction() const {
    return d_selectedAction;
}

void TitleScreen::clearAction() {
    d_selectedAction = Action::None;
}
