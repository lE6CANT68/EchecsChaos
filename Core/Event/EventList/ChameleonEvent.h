#pragma once
#include "../Core/Event/Event.h"
#include "../Audio/AudioManager.h"
#include "../Base/Board.h"
#include "../Pieces/Chameleon.h"
#include "../Engine/ScoreHandler.h"
#include <vector>
#include <cstdlib>

class ChameleonEvent : public Event {
public:
    ChameleonEvent(const AudioManager& audio) 
        : d_audio(audio) {}

    void start(Board& board) override {
        d_isActive = false; 
        
        std::vector<Position> validTargets;
        for (int x = 0; x < board.getWidth(); ++x) {
            for (int y = 0; y < board.getHeight(); ++y) {
                Position pos = {x, y};
                if (board.getTile(pos).hasPiece()) {
                    PieceType type = board.getTile(pos).getPiece()->getType();
                    if (ScoreHandler::getPieceValue(type) == 3) {
                        validTargets.push_back(pos);
                    }
                }
            }
        }
        if (!validTargets.empty()) {
            int randomIndex = rand() % validTargets.size();
            Position chosenPos = validTargets[randomIndex];
            
            PieceColor color = board.getTile(chosenPos).getPiece()->getColor();
            board.getTile(chosenPos).setPiece(std::make_unique<Chameleon>(color));
            
            d_message = "MUTATION : Une piece s'est transformee en Cameleon !";
            // d_audio.playMove(); 
        } else {
            d_message = "MUTATION RATEE : Aucune cible valide (Score 3) sur le plateau.";
        }
    }
    void step(Board& board) override {}
    
    bool isFinished() const override { return !d_isActive; }
    const char* getMessage() const override { return d_message; }
    GlobalEffect getGlobalEffect() const override { return GlobalEffect::None; }
    EventRarity getRarity() const override { return EventRarity::Legendary; }

private:
    const AudioManager& d_audio;
    
    bool d_isActive = true; 
    const char* d_message = "";
};