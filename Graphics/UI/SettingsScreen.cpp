#include "SettingsScreen.h"

SettingsScreen::SettingsScreen(Settings& settings)
    : d_settings(settings), d_selectedAction(Action::None)
{
}

void SettingsScreen::setButtonClickCallback(const std::function<void()>& callback) {
    d_buttonClickCallback = callback;
}

void SettingsScreen::update(int mouseX, int mouseY, bool isMousePressed) {
    // Update hover states
    d_resolution800x600.update(mouseX, mouseY, false);
    d_resolution1000x800.update(mouseX, mouseY, false);
    d_resolution1200x900.update(mouseX, mouseY, false);
    d_volumeDown.update(mouseX, mouseY, false);
    d_volumeUp.update(mouseX, mouseY, false);
    d_saveButton.update(mouseX, mouseY, false);
    d_backButton.update(mouseX, mouseY, false);

    if (!isMousePressed) return;

    // Gestion des clics
    if (d_resolution800x600.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_settings.setWindowWidth(800);
        d_settings.setWindowHeight(600);
    } else if (d_resolution1000x800.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_settings.setWindowWidth(1000);
        d_settings.setWindowHeight(800);
    } else if (d_resolution1200x900.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_settings.setWindowWidth(1200);
        d_settings.setWindowHeight(900);
    } else if (d_volumeDown.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        float currentVolume = d_settings.getMusicVolume();
        d_settings.setMusicVolume(std::max(0.0f, currentVolume - 0.1f));
    } else if (d_volumeUp.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        float currentVolume = d_settings.getMusicVolume();
        d_settings.setMusicVolume(std::min(1.0f, currentVolume + 0.1f));
    } else if (d_saveButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_selectedAction = Action::Save;
        d_settings.save(); // Sauvegarde immédiate
    } else if (d_backButton.update(mouseX, mouseY, true)) {
        if (d_buttonClickCallback) d_buttonClickCallback();
        d_selectedAction = Action::Back;
        d_settings.save(); //Auto save on quit
    }
}

void SettingsScreen::draw() const {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // Titre
    const char* titleText = "PARAMETRES";
    int titleWidth = MeasureText(titleText, 50);
    DrawText(titleText, (GetScreenWidth() - titleWidth) / 2, 50, 50, RAYWHITE);

    // Section Résolution
    DrawText("Resolution:", 100, 120, 30, LIGHTGRAY);
    d_resolution800x600.draw();
    d_resolution1000x800.draw();
    d_resolution1200x900.draw();

    // Section Volume
    DrawText("Volume de la musique:", 100, 220, 30, LIGHTGRAY);
    d_volumeDown.draw();
    d_volumeUp.draw();
    drawVolumeText();

    // Boutons de sauvegarde et retour
    d_saveButton.draw();
    d_backButton.draw();

    EndDrawing();
}

void SettingsScreen::drawVolumeText() const {
    int volumePercent = static_cast<int>(d_settings.getMusicVolume() * 100);
    std::string volumeText = std::to_string(volumePercent) + "%";
    int textWidth = MeasureText(volumeText.c_str(), 30);
    DrawText(volumeText.c_str(), 400 - textWidth/2, 260, 30, YELLOW);
}

SettingsScreen::Action SettingsScreen::getSelectedAction() const {
    return d_selectedAction;
}

void SettingsScreen::clearAction() {
    d_selectedAction = Action::None;
}
