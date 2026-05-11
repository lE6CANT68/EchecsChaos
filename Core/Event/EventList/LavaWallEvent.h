#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include "../Base/Board.h"
#include <vector>
#include <map>

class LavaWallEvent : public Event {
public:
    LavaWallEvent(std::vector<Position> positions, int duration, const AudioManager& audio) 
        : d_wallPositions(positions), d_duration(duration), d_audio(audio) {}

    void start(Board& board) {
    d_isActive = true;
    d_message = "ERUPTION VOLCANIQUE : Un mur de lave se dresse !";
    
     //d_audio.playExplosion() 

    for (const Position& pos : d_wallPositions) {
        if (board.isinBounds(pos)) {
            if (board.getTile(pos).hasPiece()) {
                board.getTile(pos).setPiece(nullptr); 
                d_audio.playCapture(); 
            }
            board.getTile(pos).setType(TileType::Lava);
        }
    }
}

void step(Board& board) {
    if (d_isActive) {
        d_turnsElapsed++;
        std::map<Piece*, int> currentPiecesInLava; 

        for (const Position& pos : d_wallPositions) {
            if (board.isinBounds(pos) && board.getTile(pos).hasPiece()) {
                Piece* p = board.getTile(pos).getPiece();
                int turnsBurning = d_piecesInLava[p] + 1; 
                
                if (turnsBurning >= 6) {
                    board.getTile(pos).setPiece(nullptr);
                    d_audio.playCapture(); 
                } else {
                    currentPiecesInLava[p] = turnsBurning;
                }
            }
        }
        d_piecesInLava = currentPiecesInLava; 

        if (d_turnsElapsed >= d_duration) {
            d_isActive = false;
            d_message = "Le mur de lave a refroidi.";
            for (const Position& pos : d_wallPositions) {
                if (board.isinBounds(pos) && board.getTile(pos).getType() == TileType::Lava) {
                    board.getTile(pos).setType(TileType::Normal);
                }
            }
        }
    }
}
    
    bool isFinished() const override { return !d_isActive; }
    const char* getMessage() const override { return d_message; }
    GlobalEffect getGlobalEffect() const override { return GlobalEffect::None; }

private:
    std::vector<Position> d_wallPositions;
    int d_duration;
    int d_turnsElapsed = 0;
    const AudioManager& d_audio;
    
    bool d_isActive = false;
    const char* d_message = "";

    std::map<Piece*, int> d_piecesInLava;
};