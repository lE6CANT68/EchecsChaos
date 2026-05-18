#include "GamemodeMenu.h"

GamemodeMenu::GamemodeMenu()
    : d_localButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 300, "Duel local"),
      d_aiButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 380, "Duel contre l'IA"),
      d_onlineButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 460, "Duel en ligne"),
      d_backButton(Config::Graphics::CONFIG_WINDOW_WIDTH / 2 - 100, 540, "Retour"),
      d_selection(Selection::None)
{
}

void GamemodeMenu::setButtonClickCallback(const std::function<void()>& callback) {
    d_buttonClickCallback = callback;
}

void GamemodeMenu::update(int mouseX, int mouseY, bool isMousePressed) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float centerX = screenW / 2.0f - d_localButton.getWidth() / 2.0f;
    int startY = (int)(screenH * 0.375f);
    float spacing = d_localButton.getHeight() + 20.0f;

    d_localButton.setPosition(centerX, (float)startY);
    d_aiButton.setPosition(centerX, (float)(startY + spacing));
    d_onlineButton.setPosition(centerX, (float)(startY + 2 * spacing));
    d_backButton.setPosition(centerX, (float)(startY + 3 * spacing));

    d_localButton.update(mouseX, mouseY, false);
    d_aiButton.update(mouseX, mouseY, false);
    d_onlineButton.update(mouseX, mouseY, false);
    d_backButton.update(mouseX, mouseY, false);

    if (!isMousePressed) {
        return;
    }

    if (d_localButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_selection = Selection::Local;
        return;
    }
    if (d_aiButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_selection = Selection::AI;
        return;
    }
    if (d_onlineButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_selection = Selection::Online;
        return;
    }
    if (d_backButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_selection = Selection::Back;
        return;
    }
}

void GamemodeMenu::draw() const {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    DrawRectangle(0, 0, screenW, screenH, {0, 0, 0, 150});

    const char* title = "Choisir le mode de jeu";
    const int titleSize = 36;
    int titleWidth = MeasureText(title, titleSize);
    int titleY = (int)(screenH * 0.275f);
    DrawText(title, (screenW - titleWidth) / 2, titleY, titleSize, RAYWHITE);

    d_localButton.draw();
    d_aiButton.draw();
    d_onlineButton.draw();
    d_backButton.draw();
}

GamemodeMenu::Selection GamemodeMenu::getSelection() const {
    return d_selection;
}

void GamemodeMenu::clearSelection() {
    d_selection = Selection::None;
}
