#pragma once
#include "Settings.h"
#include "Button.h"
#include <raylib.h>
#include <string>

class SettingsScreen {
public:
    enum class Action {
        None,
        Back,
        Save
    };

    SettingsScreen(Settings& settings);

    void update(int mouseX, int mouseY, bool isMousePressed);
    void draw() const;

    Action getSelectedAction() const;
    void clearAction();

private:
    Settings& d_settings;
    Action d_selectedAction;

    // Boutons de résolution
    Button d_resolution800x600{ 200, 150, "800x600" };
    Button d_resolution1000x800{ 350, 150, "1000x800" };
    Button d_resolution1200x900{ 500, 150, "1200x900" };

    // Boutons de volume
    Button d_volumeDown{ 300, 250, "-" };
    Button d_volumeUp{ 450, 250, "+" };

    // Boutons de sauvegarde et retour
    Button d_saveButton{ 250, 350, "SAUVEGARDER" };
    Button d_backButton{ 550, 350, "Annuler" };

    void drawVolumeText() const;
};
