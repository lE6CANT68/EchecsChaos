#include "EventManager.h"
#include "../Core/Event/EventList/MeteoriteEvent.h"

EventManager::EventManager(const AudioManager& audio) : d_messageTimer(0.0f), d_currentMessage(""),d_activeEvent{nullptr} {

    d_eventsList.push_back(std::make_unique<MeteoriteEvent>(audio));
}

void EventManager::update() {
    if (d_messageTimer > 0.0f) d_messageTimer -= GetFrameTime();
}

bool EventManager::hasActiveMessage() const { return d_messageTimer > 0.0f; }
const char* EventManager::getActiveMessage() const { return d_currentMessage; }

void EventManager::triggerRandomEvent(Board& board) {
    if (d_activeEvent != nullptr) {
        d_activeEvent->step(board);
        d_currentMessage = d_activeEvent->getMessage();
        d_messageTimer = 4.0f;
        if (d_activeEvent->isFinished()) {
            d_activeEvent = nullptr; 
        }
        return; 
    }

    if (d_eventsList.empty()) return;
    int randomIndex = GetRandomValue(0, d_eventsList.size() - 1);
    d_activeEvent = d_eventsList[randomIndex].get();
    d_activeEvent->start(board);
    d_currentMessage = d_activeEvent->getMessage();
    d_messageTimer = 4.0f; 
}

std::vector<VisualEffect> EventManager::getActiveVisualEffects() const {
    if (d_activeEvent != nullptr) {
        return d_activeEvent->getActiveEffects();
    }
    return {};
}