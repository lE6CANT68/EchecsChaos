#pragma once
#include "../Base/Board.h"
#include "../Audio/AudioManager.h"
#include "../Base/Board.h"
#include "../Graphics/VisualEffect.h"

enum class GlobalEffect {
    None,
    HideTimeWhite,
    HideTimeBlack,
    HideTimeBoth  
};
enum class PlayerTarget {
    White,
    Black,
    Both
};
class Event {
public:
    virtual ~Event() = default;

    virtual void start(Board& board) = 0;
    virtual void step(Board& board) = 0;
    virtual bool isFinished() const = 0;
    virtual const char* getMessage() const = 0;

    

    virtual std::vector<VisualEffect> getActiveEffects() const { return {}; }
    virtual GlobalEffect getGlobalEffect() const { return GlobalEffect::None; }
};