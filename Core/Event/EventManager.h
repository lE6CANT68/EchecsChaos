#pragma once
#include "../Base/Board.h"
#include "../Core/Time/Chrono.h"
#include "../Audio/AudioManager.h"
#include "../Core/Event/Event.h"
#include <string>
#include <functional> 
#include "raylib.h"

#include "../Core/Event/EventList/MeteoriteEvent.h"
#include "../Core/Event/EventList/FreezeEvent.h"
#include "../Core/Event/EventList/ObstacleEvent.h"
#include "../Core/Event/EventList/FogEvent.h"
#include "../Core/Event/EventList/IdiotEvent.h"
#include "../Core/Event/EventList/DuckEvent.h"
#include "../Core/Event/EventList/PionDebileEvent.h"
#include "../Core/Event/EventList/PortalEvent.h"

#include "../Core/Event/EventList/SlipperyTerrainEvent.h"
#include "../Core/Event/EventList/ExpandBoardEvent.h"
#include "../Core/Event/EventList/HideTimeEvent.h"
#include "../Core/Event/EventList/ChameleonEvent.h"
#include "../Core/Event/EventList/LavaWallEvent.h"


class EventManager {
public:
    EventManager(const AudioManager& audio);

    void update(); 
    bool hasActiveMessage() const;
    const char* getActiveMessage() const;

    void processActiveEvents(Board& board);

    void triggerRandomEvent(Board& board);

    void addEvent(Board& board, std::unique_ptr<Event> newEvent);

    std::vector<VisualEffect> getActiveVisualEffects() const;

    void setEventProbability(int proba) { d_eventProbability = proba; }

    const AudioManager& getAudio() const { return d_audio; }

    bool hasGlobalEffect(GlobalEffect effect) const;

    std::unique_ptr<Event> generateRandomEvent(EventRarity rarity);

private:
    float d_messageTimer = 0.0f;
    std::vector<Position> generateRandomWall();
    std::string d_currentMessage;

    std::vector<std::unique_ptr<Event>> d_activeEvents;

    std::vector<std::function<std::unique_ptr<Event>()>> d_eventGenerators;

    int d_eventProbability = 5; 

    const AudioManager& d_audio;
    bool d_boardHasExpanded = false; 
};