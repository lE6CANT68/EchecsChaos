#include "Settings.h"
#include <sstream>
#include <raylib.h>

Settings::Settings()
    : d_windowWidth(Config::Graphics::CONFIG_WINDOW_WIDTH),
      d_windowHeight(Config::Graphics::CONFIG_WINDOW_HEIGHT),
      d_musicVolume(Config::Audio::DEFAULT_MUSIC_VOLUME)
{
    load();
}

void Settings::load() {
    if (!std::filesystem::exists(SETTINGS_FILE)) {
        createDefaultSettings();
        save();
        return;
    }

    std::ifstream file(SETTINGS_FILE);
    if (!file.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier settings.ini" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        parseLine(line);
    }
    file.close();
}

void Settings::save() const {
    std::ofstream file(SETTINGS_FILE);
    if (!file.is_open()) {
        std::cerr << "Erreur: Impossible de créer le fichier settings.ini" << std::endl;
        return;
    }

    file << "window_width=" << d_windowWidth << std::endl;
    file << "window_height=" << d_windowHeight << std::endl;
    file << "music_volume=" << d_musicVolume << std::endl;

    file.close();
}

void Settings::applySettings() {
    SetWindowSize(d_windowWidth, d_windowHeight);
    // Le volume sera appliqué par AudioManager
}

void Settings::backupCurrentSettings() {
    d_backupWindowWidth = d_windowWidth;
    d_backupWindowHeight = d_windowHeight;
    d_backupMusicVolume = d_musicVolume;
}

void Settings::restoreSettings() {
    d_windowWidth = d_backupWindowWidth;
    d_windowHeight = d_backupWindowHeight;
    d_musicVolume = d_backupMusicVolume;
}

void Settings::createDefaultSettings() {
    d_windowWidth = Config::Graphics::CONFIG_WINDOW_WIDTH;
    d_windowHeight = Config::Graphics::CONFIG_WINDOW_HEIGHT;
    d_musicVolume = Config::Audio::DEFAULT_MUSIC_VOLUME;
}

void Settings::parseLine(const std::string& line) {
    if (line.empty() || line[0] == '#') return;

    size_t pos = line.find('=');
    if (pos == std::string::npos) return;

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    if (key == "window_width") {
        d_windowWidth = std::stoi(value);
    } else if (key == "window_height") {
        d_windowHeight = std::stoi(value);
    } else if (key == "music_volume") {
        d_musicVolume = std::stof(value);
    }
}
