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
    void playButtonPress() const;
    void playBaseMusic() const;
    void playGeminiWaitMusic();
    void stopCurrentMusic() const;
    void playGameOver() const;
    void playVictory() const;
    void playStalemate() const;

    void updateMusicStream() const;
    void setVolume(float volume);

private:
    Sound d_soundMove;
    Sound d_soundCapture;
    Sound d_soundButtonPress;
    Sound d_soundVictory;
    Sound d_soundGameOver;
    Sound d_soundStalemate;
    float d_volume;
    
    Music d_baseMusic;
    Music d_geminiMusic;
    bool d_isGeminiMusicActive = false;
};