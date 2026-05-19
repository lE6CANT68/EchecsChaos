#pragma once
#include "../Event.h"
#include "../Audio/AudioManager.h"
#include "../../Pieces/Duck.h"
#include <vector>
#include <string>
#include "raylib.h"

class DuckEvent : public Event {
public:
    DuckEvent(const AudioManager& audio) : d_audio(audio), d_turnCount(0) {}

    void start(Board& board) override {
        // Vérifier si un canard existe déjà
        for (int y = 0; y < board.getHeight(); ++y) {
            for (int x = 0; x < board.getWidth(); ++x) {
                Position pos = {x, y};
                const Tile& tile = board.getTile(pos);
                
                if (tile.hasPiece() && tile.getPiece()->getType() == PieceType::Duck) {
                    d_messageStr = "Un canard fou est deja present !";
                    d_isActive = true; 
                    return; 
                }
            }
        }

        // Chercher des cases vides dans les lignes centrales
        std::vector<Position> emptyPositions;
        int midRow1 = (board.getHeight() / 2) - 1;
        int midRow2 = board.getHeight() / 2;

        for (int y = midRow1; y <= midRow2; ++y) {
            for (int x = 0; x < board.getWidth(); ++x) {
                Position pos = {x, y};
                // Ne pas spawner sur les portails
                if (!board.getTile(pos).hasPiece() && !board.isPortal(pos)) {
                    emptyPositions.push_back(pos);
                }
            }
        }

        if (emptyPositions.empty()) {
            d_messageStr = "Le canard fou ne peut pas arriver, l'espace est occupe !";
            d_isActive = true;
            return;
        }

        // Placer le canard à une position aléatoire
        int randomIndex = GetRandomValue(0, emptyPositions.size() - 1);
        d_duckPos = emptyPositions[randomIndex];
        
        auto newDuck = std::make_unique<Duck>(PieceColor::Neutral);
        board.getTile(d_duckPos).setPiece(std::move(newDuck));
        
        d_messageStr = "Un canard fou apparait !";
        d_isActive = false; // L'événement se termine après la spawn
        d_audio.playMove();
    }

    void step(Board& board) override {
        // Le canard ne se déplace plus ici - GameEngine gère le mouvement du canard
        // pendant son tour comme troisième joueur
    }

    std::vector<VisualEffect> getActiveEffects() const override {
        // Le canard n'a plus d'effet visuel spécial
        return {};
    }

    bool isFinished() const override { 
        // L'événement se termine après la spawn du canard
        // GameEngine gère maintenant le mouvement du canard
        return !d_isActive;
    }
    
    const char* getMessage() const override { return d_messageStr.c_str(); }
    EventRarity getRarity() const override { return EventRarity::Epic; }

private:
    const AudioManager& d_audio;
    Position d_duckPos = {-1, -1};
    bool d_isActive = false;
    std::string d_messageStr = "";
    int d_turnCount = 0;
};
