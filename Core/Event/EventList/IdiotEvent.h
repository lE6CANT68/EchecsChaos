#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include "../Core/Pieces/Idiot.h" 
#include <vector>
#include <string>
#include "raylib.h"

class IdiotEvent : public Event {
public:
    IdiotEvent(const AudioManager& audio) : d_audio(audio) {}
    
    void start(Board& board) override {
        for (int y = 0; y < board.getHeight(); ++y) {
            for (int x = 0; x < board.getWidth(); ++x) {
                Position pos = {x, y};
                const Tile& tile = board.getTile(pos);
                
                if (tile.hasPiece() && tile.getPiece()->getType() == PieceType::Idiot) {
                    d_messageStr = "Un Idiot est deja la, il fait de son mieux !";
                    d_isActive = true; 
                    return; 
                }
            }
        }
        // ------------------------------------

        std::vector<Position> emptyPositions;
        
        int ligneMilieuHaut = (board.getHeight() / 2) - 1; 
        int ligneMilieuBas = board.getHeight() / 2;

        for (int y = ligneMilieuHaut; y <= ligneMilieuBas; ++y) {
            for (int x = 0; x < board.getWidth(); ++x) {
                Position pos = {x, y};
                // Ne pas spawner sur les portails
                if (!board.getTile(pos).hasPiece() && !board.isPortal(pos)) {
                    emptyPositions.push_back(pos);
                }
            }
        }

        if (emptyPositions.empty()) {
            d_messageStr = "L'Idiot voulait venir, mais le centre est plein !";
            d_isActive = true; 
            return;
        }

        int randomIndex = GetRandomValue(0, emptyPositions.size() - 1);
        d_target = emptyPositions[randomIndex];
        auto nouvelIdiot = std::make_unique<Idiot>(PieceColor::Neutral);

        board.getTile(d_target).setPiece(std::move(nouvelIdiot));
        d_audio.playMove(); 
        d_messageStr = "Un Idiot du Village apparait au centre du plateau !";
        d_isActive = true;
    }

    void step(Board& board) override {
        if (d_isActive) {
            d_isActive = false;
        }
    }

    std::vector<VisualEffect> getActiveEffects() const override {
        if (d_isActive && d_target.x != -1) 
        {
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
    std::string d_messageStr = "";
};