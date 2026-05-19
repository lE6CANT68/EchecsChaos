#pragma once
#include "../Event.h"
#include "../Audio/AudioManager.h"
#include <vector>
#include <string>
#include "raylib.h"

class PortalEvent : public Event {
public:
    PortalEvent(const AudioManager& audio) : d_audio(audio) {}

    void start(Board& board) override {
        // Vérifier s'il y a déjà des portails
        if (board.isPortal({-1, -1}) == false && (board.isPortal(board.getPortalExit({-1, -1})))) {
            // On suppose que la méthode isPortal retourne true si un portail existe
            d_messageStr = "Il y a déjà des portails sur le plateau !";
            d_isActive = true;
            return;
        }

        int w = board.getWidth();
        int h = board.getHeight();
        int mid1 = h / 2 - 1;
        int mid2 = h / 2;
        int mid3 = (h % 2 == 0) ? (h / 2 + 1) : (h / 2 + 1);
        std::vector<Position> candidates;
        for (int y : {mid1, mid2, mid3}) {
            if (y < 0 || y >= h) continue;
            for (int x = 0; x < w; ++x) {
                Position pos = {x, y};
                if (!board.getTile(pos).hasPiece() && !board.isPortal(pos)) {
                    candidates.push_back(pos);
                }
            }
        }
        if (candidates.size() < 2) {
            d_messageStr = "Pas assez de place pour placer deux portails !";
            d_isActive = true;
            return;
        }
        int idx1 = GetRandomValue(0, candidates.size() - 1);
        Position p1 = candidates[idx1];
        candidates.erase(candidates.begin() + idx1);
        int idx2 = GetRandomValue(0, candidates.size() - 1);
        Position p2 = candidates[idx2];
        board.setPortals(p1, p2);
        d_messageStr = "Deux portails apparaissent au centre du plateau !";
        d_isActive = true;
        d_audio.playMove();
    }

    void step(Board& board) override {
        d_isActive = false;
    }

    std::vector<VisualEffect> getActiveEffects() const override {
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
    bool d_isActive = false;
    std::string d_messageStr = "";
};
