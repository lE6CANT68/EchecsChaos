#include "AudioManager.h"
AudioManager::AudioManager() {
    d_soundMove = LoadSound(Audiopath::SOUND_MOVE);
    d_soundCapture = LoadSound(Audiopath::SOUND_CAPTURE);
    //d_soundCheck = LoadSound(Audiopath::SOUND_CHECK);
    d_soundGameOver = LoadSound(Audiopath::SOUND_GAME_OVER);
    d_soundVictory = LoadSound(Audiopath::SOUND_VICTORY);
    d_soundStalemate =LoadSound(Audiopath::SOUND_STALEMATE);
    d_music = LoadMusicStream(Audiopath::GAME_MUSIC);
    
    d_volume = Config::Audio::DEFAULT_MUSIC_VOLUME;
    SetMusicVolume(d_music, d_volume); //chnager pour pas fixer
    PlayMusicStream(d_music);      
}

AudioManager::~AudioManager() {
    UnloadSound(d_soundMove);
    UnloadSound(d_soundCapture);
    
    UnloadMusicStream(d_music);
}

void AudioManager::playMove() const { PlaySound(d_soundMove); }
void AudioManager::playCapture() const { PlaySound(d_soundCapture); }
void AudioManager::playMusic() const{PlayMusicStream(d_music);}
void AudioManager::playGameOver() const{PlaySound(d_soundGameOver);}
void AudioManager::playVictory() const{PlaySound(d_soundVictory);}
void AudioManager::playStalemate() const{PlaySound(d_soundStalemate);}
void AudioManager::setVolume(float volume) {
    d_volume = volume;
    SetMusicVolume(d_music, d_volume);
}

