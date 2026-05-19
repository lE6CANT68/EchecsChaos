#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include "../Base/Board.h"

class HideTimeEvent : public Event {
public:
    HideTimeEvent(PlayerTarget target, int duration, const AudioManager& audio) 
        : d_target(target), d_duration(duration), d_audio(audio) {}

    void start(Board& board) override {
        d_isActive = true;
        if (d_target == PlayerTarget::Both) {
            d_message = "SABOTAGE TEMPOREL : Tous les chronos sont masques !";
        } else {
            d_message = "SABOTAGE TEMPOREL : Le chrono ennemi est masque !";
        }
    }

    void step(Board& board) override {
        if (d_isActive) {
            d_turnsElapsed++;
            if (d_turnsElapsed >= d_duration) {
                d_isActive = false;
                d_message = "La vision temporelle ennemie est retablie.";
            }
        }
    }

    bool isFinished() const override { return !d_isActive; }
    const char* getMessage() const override { return d_message; }

    GlobalEffect getGlobalEffect() const override { 
        if (!d_isActive) return GlobalEffect::None;
        
        if (d_target == PlayerTarget::White) return GlobalEffect::HideTimeWhite;
        if (d_target == PlayerTarget::Black) return GlobalEffect::HideTimeBlack;
        return GlobalEffect::HideTimeBoth;
    }
    EventRarity getRarity() const override { return EventRarity::Epic; }

private:
    PlayerTarget d_target;
    int d_duration;
    int d_turnsElapsed = 0;
    const AudioManager& d_audio;
    
    bool d_isActive = false;
    const char* d_message = "";
};