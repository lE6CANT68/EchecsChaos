#pragma once
#include "raylib.h"
#include "../Core/Config/AudioPath.h"
#include "../Config/Constant.h"

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    
    void playMove() const;
    void playCapture() const;
    void playMusic() const;
    void playGameOver() const;
    void playVictory() const;
    void playStalemate() const;

    void setVolume(float volume);

    Music getMusic() const { return d_music; }

private:
    Sound d_soundMove;
    Sound d_soundCapture;
    Sound d_soundVictory;
    Sound d_soundGameOver;
    Sound d_soundStalemate;
    float d_volume;
    

    Music d_music;
};