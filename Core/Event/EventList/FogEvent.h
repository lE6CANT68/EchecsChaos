#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include "../Base/Board.h"
#include <vector>

class FogEvent : public Event {
public:
    FogEvent(Position target, int duration, const AudioManager& audio) 
        : d_target(target), d_duration(duration), d_audio(audio) {}

    void start(Board& board) override {
        d_isActive = true;

        if (d_target.x == -1 && d_target.y == -1) {
            d_message = "BROUILLARD DE GUERRE : Le plateau entier disparait !";
            for (int x = 0; x < board.getWidth(); ++x) {
                for (int y = 0; y < board.getHeight(); ++y) {
                    Position p = {x, y};
                    board.getTile(p).setFoggy(true);
                    d_affectedTiles.push_back(p);
                }
            }
        } 
        else {
            d_message = "BOMBE FUMIGENE : Une zone 3x3 est obscurcie !";
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    Position p = {d_target.x + dx, d_target.y + dy};

                    if (board.isinBounds(p)) {
                        board.getTile(p).setFoggy(true);
                        d_affectedTiles.push_back(p);
                    }
                }
            }
        }
    }

    void step(Board& board) override {
        if (d_isActive) {
            d_turnsElapsed++;
            if (d_turnsElapsed >= d_duration) {
                for (Position p : d_affectedTiles) {
                    board.getTile(p).setFoggy(false);
                }
                d_isActive = false;
                d_message = "Le brouillard se dissipe.";
            }
        }
    }

    bool isFinished() const override { return !d_isActive; }
    const char* getMessage() const override { return d_message; }

    EventRarity getRarity() const override { return EventRarity::Rare; }

private:
    Position d_target;
    int d_duration;
    int d_turnsElapsed = 0;
    const AudioManager& d_audio;
    
    std::vector<Position> d_affectedTiles; 
    bool d_isActive = false;
    const char* d_message = "";
};