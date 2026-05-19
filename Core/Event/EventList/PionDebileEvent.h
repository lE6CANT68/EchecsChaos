#pragma once
#include "../Event.h"
#include "../Audio/AudioManager.h"
#include "../Core/Pieces/PionDebile.h"
#include <vector>
#include <string>
#include "raylib.h"

class PionDebileEvent : public Event {
public:
    PionDebileEvent(const AudioManager& audio) : d_audio(audio) {}
    
    void start(Board& board) override {
        // Chercher tous les pions qui peuvent être transformés
        std::vector<Position> pawns;
        
        for (int x = 0; x < board.getWidth(); ++x) {
            for (int y = 0; y < board.getHeight(); ++y) {
                Position pos = {x, y};
                const Tile& tile = board.getTile(pos);
                
                if (tile.hasPiece() && tile.getPiece()->getType() == PieceType::Pawn) {
                    pawns.push_back(pos);
                }
            }
        }

        if (pawns.empty()) {
            d_messageStr = "Il n'y a pas de pions a rendre debiles !";
            d_isActive = true;
            return;
        }

        // Choisir un pion aléatoire
        int randomIndex = GetRandomValue(0, pawns.size() - 1);
        d_target = pawns[randomIndex];
        
        // Récupérer la couleur du pion avant de le remplacer
        PieceColor pawnColor = board.getTile(d_target).getPiece()->getColor();
        
        // Remplacer le pion par un pion débile
        board.getTile(d_target).setPiece(std::make_unique<PionDebile>(pawnColor));
        
        d_audio.playMove();
        d_messageStr = "Un Pion est devenu Debile !";
        d_isActive = true;
    }

    void step(Board& board) override {
        if (d_isActive) {
            d_isActive = false;
        }
    }

    std::vector<VisualEffect> getActiveEffects() const override {
        if (d_isActive && d_target.x != -1) {
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
        return EventRarity::Common;
    }

private:
    const AudioManager& d_audio;
    Position d_target = {-1, -1};
    bool d_isActive = false;
    std::string d_messageStr = "";
};
