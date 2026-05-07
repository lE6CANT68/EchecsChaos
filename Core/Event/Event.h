#pragma once
#include "../Base/Board.h"
#include "../Audio/AudioManager.h"
#include "../Base/Board.h"
#include "../Graphics/VisualEffect.h"

enum class EventRarity {
    Common,     // Gris 
    Rare,       // Bleu 
    Epic,       // Violet 
    Legendary   // Doré 
};

class Event {
public:
    virtual ~Event() = default;

    virtual void start(Board& board) = 0;
    virtual void step(Board& board) = 0;
    virtual bool isFinished() const = 0;
    virtual const char* getMessage() const = 0;
    virtual EventRarity getRarity() const = 0;   

    virtual std::vector<VisualEffect> getActiveEffects() const { return {}; }
};