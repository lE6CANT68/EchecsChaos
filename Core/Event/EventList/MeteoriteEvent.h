#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include <vector>
#include <string> 
#include "raylib.h"
#include <cmath>

class MeteoriteEvent : public Event {
public:
    MeteoriteEvent(const AudioManager& audio) : d_audio(audio) {}
    
    void start(Board& board) override {
        int randomx = GetRandomValue(0, board.getWidth() - 1);
        int randomy = GetRandomValue(0, board.getHeight() - 1);
        d_target = {randomx, randomy}; 
        
        d_isActive = true;
        
        d_stepsRemaining = 4; 
        d_messageStr = "ALERTE METEORITE ! Impact dans 2 tours !";
    }

    void step(Board& board) override {
        if (!d_isActive) return;

        d_stepsRemaining--;

        if (d_stepsRemaining > 0) {
            int toursRestants = (d_stepsRemaining + 1) / 2; 
            d_messageStr = "ALERTE METEORITE ! Impact dans " + std::to_string(toursRestants) + " tour(s) !";
        } 
        else {
            Tile& targetTile = board.getTile(d_target);
            if (targetTile.hasPiece()) {
                if (targetTile.getPiece()->getType() != PieceType::King) {
                    targetTile.setPiece(nullptr); 
                    d_audio.playCapture(); 
                    d_messageStr = "BOUM ! Une piece a ete pulverisee.";
                } else {
                    d_audio.playMove(); 
                    d_messageStr = "BOUM ! Le Roi a survecu a l'impact !";
                }
            } else {
                d_audio.playMove(); 
                d_messageStr = "BOUM ! La meteorite s'est ecrasee dans le vide.";
            }
            
            d_isActive = false; 
        }
    }

    std::vector<VisualEffect> getActiveEffects() const override {
        if (d_isActive) {
            return { {d_target, EffectType::MeteoriteWarning} };
        }
        return {}; 
    }

    bool isFinished() const override { 
        return !d_isActive; 
    }

    const char* getMessage() const override { 
        return d_messageStr.c_str(); 
    }

    EventRarity getRarity() const override {
        return EventRarity::Rare; 
    }

private:
    const AudioManager& d_audio;
    Position d_target = {-1, -1};
    bool d_isActive = false;
    
    int d_stepsRemaining = 0; 
    std::string d_messageStr = ""; 
};