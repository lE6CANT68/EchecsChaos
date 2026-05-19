#pragma once
#include "../Core/Event/Event.h"
#include "../Base/Piece.h"

class PawnSprintEvent : public Event {
public:
    PawnSprintEvent(Piece* targetPawn) : d_pawn(targetPawn) {}

    void start(Board& board) override {
        if (d_pawn) {
            d_pawn->setHasMoved(false); 
            d_isActive = true;
            d_message = "ELAN HEROIQUE : Ce pion peut a nouveau bondir !";
        }
    }

    void step(Board& board) override { d_isActive = false; }
    bool isFinished() const override { return !d_isActive; }
    const char* getMessage() const override { return d_message; }
    
    EventRarity getRarity() const override {
        return EventRarity::Common; 
    }
private:
    Piece* d_pawn;
    bool d_isActive = false;
    const char* d_message = "";
};