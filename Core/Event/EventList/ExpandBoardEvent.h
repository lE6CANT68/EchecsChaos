#pragma once
#include "../Event.h"
#include "../Audio/AudioManager.h"
#include <vector>
#include <string>
#include "raylib.h"

class ExpandBoardEvent : public Event {
public:
    ExpandBoardEvent(const AudioManager& audio) : d_audio(audio), d_isActive(false) {}

    void start(Board& board) override {
        // Agrandir le plateau de 8x8 à 9x9
        board.expandBoard();
        
        d_messageStr = "Le plateau s'agrandit! De 8x8 à 9x9 !";
        d_isActive = true;
        d_audio.playMove();
    }

    void step(Board& board) override {
        // L'événement ne fait que l'expansion au démarrage
        if (d_isActive) {
            d_isActive = false;
        }
    }

    std::vector<VisualEffect> getActiveEffects() const override {
        // Pas d'effet visuel spécial
        return {};
    }

    bool isFinished() const override { 
        // L'événement se termine immédiatement après l'expansion
        return !d_isActive;
    }
    
    const char* getMessage() const override { 
        return d_messageStr.c_str(); 
    }
    
    EventRarity getRarity() const override {
        return EventRarity::Legendary; 
    }

private:
    const AudioManager& d_audio;
    bool d_isActive;
    std::string d_messageStr = "";
};
