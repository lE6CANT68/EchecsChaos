#include "AudioManager.h"
AudioManager::AudioManager() {
    d_soundMove = LoadSound(Audiopath::SOUND_MOVE);
    d_soundCapture = LoadSound(Audiopath::SOUND_CAPTURE);
    d_soundButtonPress = LoadSound(Audiopath::SOUND_BUTTON_PRESS);
    d_soundGameOver = LoadSound(Audiopath::SOUND_GAME_OVER);
    d_soundVictory = LoadSound(Audiopath::SOUND_VICTORY);
    d_soundStalemate = LoadSound(Audiopath::SOUND_STALEMATE);
    d_baseMusic = LoadMusicStream(Audiopath::GAME_MUSIC);
    d_geminiMusic = LoadMusicStream(Audiopath::GEMINI_WAIT_MUSIC);
    d_baseMusic.looping = true;
    d_geminiMusic.looping = true;
    
    d_volume = Config::Audio::DEFAULT_MUSIC_VOLUME;
    SetMusicVolume(d_baseMusic, d_volume);
    SetMusicVolume(d_geminiMusic, d_volume);
    PlayMusicStream(d_baseMusic);
}

AudioManager::~AudioManager() {
    UnloadSound(d_soundMove);
    UnloadSound(d_soundCapture);
    UnloadSound(d_soundButtonPress);
    UnloadSound(d_soundGameOver);
    UnloadSound(d_soundVictory);
    UnloadSound(d_soundStalemate);
    
    UnloadMusicStream(d_baseMusic);
    UnloadMusicStream(d_geminiMusic);
}

void AudioManager::playMove() const { PlaySound(d_soundMove); }
void AudioManager::playCapture() const { PlaySound(d_soundCapture); }
void AudioManager::playButtonPress() const { PlaySound(d_soundButtonPress); }
void AudioManager::playBaseMusic() const {
    if (IsMusicStreamPlaying(d_geminiMusic)) {
        StopMusicStream(d_geminiMusic);
    }
    if (IsMusicStreamPlaying(d_baseMusic)) {
        ResumeMusicStream(d_baseMusic);
    } else {
        PlayMusicStream(d_baseMusic);
    }
}
void AudioManager::playGeminiWaitMusic() {
    if (IsMusicStreamPlaying(d_baseMusic)) {
        PauseMusicStream(d_baseMusic);
    }
    if (!IsMusicStreamPlaying(d_geminiMusic)) {
        PlayMusicStream(d_geminiMusic);
    }
    d_isGeminiMusicActive = true;
}
void AudioManager::stopCurrentMusic() const {
    if (IsMusicStreamPlaying(d_baseMusic)) {
        StopMusicStream(d_baseMusic);
    }
    if (IsMusicStreamPlaying(d_geminiMusic)) {
        StopMusicStream(d_geminiMusic);
    }
}
void AudioManager::playGameOver() const { PlaySound(d_soundGameOver); }
void AudioManager::playVictory() const { PlaySound(d_soundVictory); }
void AudioManager::playStalemate() const { PlaySound(d_soundStalemate); }
void AudioManager::updateMusicStream() const {
    UpdateMusicStream(d_baseMusic);
    UpdateMusicStream(d_geminiMusic);
}
void AudioManager::setVolume(float volume) {
    d_volume = volume;
    SetMusicVolume(d_baseMusic, d_volume);
    SetMusicVolume(d_geminiMusic, d_volume);
}

