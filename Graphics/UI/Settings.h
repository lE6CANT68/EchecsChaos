#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "../Config/Constant.h"

class Settings {
public:
    Settings();

    void load();
    void save() const;

    // Getters
    int getWindowWidth() const { return d_windowWidth; }
    int getWindowHeight() const { return d_windowHeight; }
    float getMusicVolume() const { return d_musicVolume; }

    // Setters
    void setWindowWidth(int width) { d_windowWidth = width; }
    void setWindowHeight(int height) { d_windowHeight = height; }
    void setMusicVolume(float volume) { d_musicVolume = volume; }

    // Utility
    void applySettings();
    void backupCurrentSettings(); // Sauvegarder l'état actuel
    void restoreSettings(); // Restaurer l'état sauvegardé

private:
    static constexpr const char* SETTINGS_FILE = "settings.ini";

    int d_windowWidth;
    int d_windowHeight;
    float d_musicVolume;

    // Valeurs sauvegardées pour annulation
    int d_backupWindowWidth;
    int d_backupWindowHeight;
    float d_backupMusicVolume;

    void createDefaultSettings();
    void parseLine(const std::string& line);
};
