#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include "../Base/Board.h"
#include <vector>

class ObstacleEvent : public Event {
public:
    ObstacleEvent(Position target, int duration, const AudioManager& audio) 
        : d_target(target), d_duration(duration), d_audio(audio) {}

    void start(Board& board) override {
        if (d_target.x == -1 && d_target.y == -1) {
            std::vector<Position> validTargets;
            for (int x = 0; x < board.getWidth(); ++x) {
                for (int y = 0; y < board.getHeight(); ++y) {
                    Position pos = {x, y};
                    const Tile& tile = board.getTile(pos);
                    
                    if (!tile.hasPiece() && tile.getType() == TileType::Normal) {
                        validTargets.push_back(pos);
                    }
                }
            }

            if (!validTargets.empty()) {
                int randomIndex = GetRandomValue(0, validTargets.size() - 1);
                d_target = validTargets[randomIndex];
            } else {
                d_isActive = false;
                return;
            }
        }
        board.getTile(d_target).setType(TileType::Blocked);
        d_isActive = true;
        d_message = "MUR DE PIERRE : Une case a ete bloquee !";
        
        // d_audio.playObstacleSound(); 
    }

    void step(Board& board) override {
        if (d_isActive) {
            d_turnsElapsed++;
            if (d_turnsElapsed >= d_duration) {
                board.getTile(d_target).setType(TileType::Normal);
                d_isActive = false;
                d_message = "La case est maintenant libre.";
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
    
    bool d_isActive = false;
    const char* d_message = "";
};