#pragma once
#include "../Base/Board.h"
#include "../Core/Time/Chono.h"
#include "../Audio/AudioManager.h"
#include "../Core/Event/Event.h"
#include <string>

class EventManager {
public:
    EventManager(const AudioManager& audio);
    
    void update();
    bool hasActiveMessage() const;
    const char* getActiveMessage() const;

    void triggerRandomEvent(Board& board);
    std::vector<VisualEffect>getActiveVisualEffects() const;

private:
    float d_messageTimer;
    const char* d_currentMessage;
    std::vector<std::unique_ptr<Event>> d_eventsList; 
    Event* d_activeEvent ;
};