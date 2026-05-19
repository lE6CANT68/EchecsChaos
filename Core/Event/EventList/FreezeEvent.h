#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include "../Base/Board.h"

class FreezeEvent : public Event {
public:
    FreezeEvent(Position target, int duration, const AudioManager& audio) 
        : d_target(target), d_duration(duration), d_audio(audio) {}

    void start(Board& board) override {
    if (d_target.x == -1 && d_target.y == -1) {
        std::vector<Position> potentialTargets;
        for (int x = 0; x < board.getWidth(); ++x) {
            for (int y = 0; y < board.getHeight(); ++y) {
                Position p = {x, y};
                if (board.getTile(p).hasPiece() && board.getTile(p).getPiece()->getType() != PieceType::King) {
                    potentialTargets.push_back(p);
                }
            }
        }

        if (!potentialTargets.empty()) {
            d_target = potentialTargets[GetRandomValue(0, potentialTargets.size() - 1)];
        } else {
            d_isActive = false; 
            return;
        }
    }
    board.getTile(d_target).setType(TileType::Frozen);
    d_isActive = true;
    d_message = "Y fait froid par ici hein !";
}

    void step(Board& board) override {
        if (d_isActive) {
            d_turnsElapsed++;
            if (d_turnsElapsed >= d_duration) {
                board.getTile(d_target).setType(TileType::Normal);
                d_isActive = false;
                d_message = "La piece est de nouveau libre.";
            }
        }
    }

    bool isFinished() const override { return !d_isActive; }
    const char* getMessage() const override { return d_message; }
    EventRarity getRarity() const override { return EventRarity::Common; }

    EventRarity getRarity() const override {
        return EventRarity::Common; 
    }
    
private:
    Position d_target;
    int d_duration;
    int d_turnsElapsed = 0;
    const AudioManager& d_audio;
    
    bool d_isActive = false;
    const char* d_message = "";
};