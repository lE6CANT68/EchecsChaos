#include "EventManager.h"

#include "raylib.h"

EventManager::EventManager(const AudioManager& audio) 
    : d_messageTimer(0.0f), d_currentMessage(""), d_audio(audio) 
{
    d_eventGenerators.push_back([this]() {
        return std::make_unique<MeteoriteEvent>(this->d_audio);
    });
    d_eventGenerators.push_back([this]() {
        return std::make_unique<FreezeEvent>(Position::NONE, Config::Event::FREEZE_EVENT_DURATION, this->d_audio);
    });
    d_eventGenerators.push_back([this]() {
        return std::make_unique<ObstacleEvent>(Position::NONE, Config::Event::OBSTACLE_EVENT_DURATION, this->d_audio);
    });
    d_eventGenerators.push_back([this]() {
        return std::make_unique<FogEvent>(Position::NONE, Config::Event::FOG_EVENT_DURATION, this->d_audio);
    });
    d_eventGenerators.push_back([this]() {
        return std::make_unique<HideTimeEvent>(PlayerTarget::Both, Config::Event::HIDE_TIME_EVENT_DURATION, this->d_audio);
    });
    
}

void EventManager::update() {
    if (d_messageTimer > 0.0f) d_messageTimer -= GetFrameTime();
}

bool EventManager::hasActiveMessage() const { return d_messageTimer > 0.0f; }
const char* EventManager::getActiveMessage() const { return d_currentMessage; }

void EventManager::processActiveEvents(Board& board) {
    for (auto it = d_activeEvents.begin(); it != d_activeEvents.end(); ) {
        (*it)->step(board); 

        if ((*it)->isFinished()) {
            it = d_activeEvents.erase(it);
        } else {
            ++it;
        }
    }
}

void EventManager::triggerRandomEvent(Board& board) {
    if (d_eventGenerators.empty()) return;

    int chance = GetRandomValue(1, 100);
    if (chance > d_eventProbability) {
        return; 
    }

    int randomIndex = GetRandomValue(0, d_eventGenerators.size() - 1);

    std::unique_ptr<Event> newEvent = d_eventGenerators[randomIndex]();
    addEvent(board, std::move(newEvent));
}
void EventManager::addEvent(Board& board, std::unique_ptr<Event> newEvent) {
    newEvent->start(board); 
    
    d_currentMessage = newEvent->getMessage();
    d_messageTimer = Config::Graphics::MESSAGE_DISPLAY_TIME;

    d_activeEvents.push_back(std::move(newEvent));
}

std::vector<VisualEffect> EventManager::getActiveVisualEffects() const {
    std::vector<VisualEffect> allEffects;

    for (const auto& event : d_activeEvents) {
        auto effects = event->getActiveEffects();
        allEffects.insert(allEffects.end(), effects.begin(), effects.end());
    }
    
    return allEffects;
}
bool EventManager::hasGlobalEffect(GlobalEffect effect) const {
    if (effect == GlobalEffect::None) return false;
    for (const auto& event : d_activeEvents) {
        if (event->getGlobalEffect() == effect) return true;
    }
    return false;
}