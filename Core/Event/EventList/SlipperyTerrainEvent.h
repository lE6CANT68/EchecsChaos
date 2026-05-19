#pragma once
#include "../Event.h"
#include "../Audio/AudioManager.h"
#include <vector>
#include <string>
#include "raylib.h"

class SlipperyTerrainEvent : public Event {
public:
    SlipperyTerrainEvent(const AudioManager& audio) : d_audio(audio) {}

    void start(Board& board) override {
        // Activer le terrain glissant pour 2 tours (1 par joueur)
        board.setSlipperyTerrainActive(true, 2);
        
        d_messageStr = "Le sol est gele ! Toutes les pieces glissent vers l'avant !";
        d_isActive = true;
        d_audio.playMove(); 
    }

    void step(Board& board) override {
        // Diminuer le compteur à chaque tour
        board.decrementSlipperyTurns();
        
        // Si terminé, désactiver
        if (board.getSlipperyTurnsRemaining() == 0) {
            d_isActive = false;
        }
    }

    std::vector<VisualEffect> getActiveEffects() const override {
        return {};
    }

    bool isFinished() const override { 
        return !d_isActive;
    }
    
    const char* getMessage() const override { return d_messageStr.c_str(); }
    EventRarity getRarity() const override { return EventRarity::Rare; }

private:
    const AudioManager& d_audio;
    bool d_isActive = false;
    std::string d_messageStr = "";
};