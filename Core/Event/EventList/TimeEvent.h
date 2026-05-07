#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include "../Base/Player.h" // Nécessaire pour modifier le temps du joueur

class TimeEvent : public Event {
public:
    TimeEvent(Player& targetPlayer, float timeToAdd, const AudioManager& audio) 
        : d_targetPlayer(targetPlayer), d_timeToAdd(timeToAdd), d_audio(audio) {}

    void start(Board& board) override {
        d_targetPlayer.getClock().addTime(d_timeToAdd); 
        
        
        // d_audio.playPowerUp(); 
        
        d_isActive = true;
        d_message = "SABLIER MAGIQUE : +30 secondes accordees !";
    }

    void step(Board& board) override {
        if (d_isActive) {
            d_isActive = false; 
        }
    }

    bool isFinished() const override { 
        return !d_isActive; 
    }

    const char* getMessage() const override { 
        return d_message; 
    }

    EventRarity getRarity() const override {
        return EventRarity::Epic; 
    }

private:
    Player& d_targetPlayer;
    float d_timeToAdd;
    const AudioManager& d_audio;
    
    bool d_isActive = false;
    const char* d_message = "";
};