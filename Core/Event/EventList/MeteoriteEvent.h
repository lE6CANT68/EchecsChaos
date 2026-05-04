#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include <vector>
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
        d_message = "ALERTE METEORITE ! Impact au prochain tour !";
    }

    void step(Board& board) override {
        if (d_isActive) {
            Tile& targetTile = board.getTile(d_target);
            if (targetTile.hasPiece()) {
                if (targetTile.getPiece()->getType() != PieceType::King) {
                    targetTile.setPiece(nullptr); 
                    d_audio.playCapture(); 
                    d_message = "BOUM ! Une piece a ete pulverisee.";
                } else {
                    d_audio.playMove(); 
                    d_message = "BOUM ! Le Roi a survecu a l'impact !";
                }
            } else {
                // La case était vide
                d_audio.playMove(); 
                d_message = "BOUM ! La meteorite s'est ecrasee dans le vide.";
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
        return d_message; 
    }

private:
    const AudioManager& d_audio;
    Position d_target = {-1, -1};
    bool d_isActive = false;
    const char* d_message = "";
};