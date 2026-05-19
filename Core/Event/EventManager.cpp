#include "EventManager.h"

#include "raylib.h"

EventManager::EventManager(const AudioManager& audio) 
    : d_messageTimer(0.0f), d_currentMessage(""), d_audio(audio) 
{
   // d_eventGenerators.push_back([this]() {
    //    return std::make_unique<MeteoriteEvent>(this->d_audio);
   //});
   d_eventGenerators.push_back([this]() {
        return std::make_unique<IdiotEvent>(this->d_audio);
   });
   d_eventGenerators.push_back([this]() {
        return std::make_unique<DuckEvent>(this->d_audio);
   });
   d_eventGenerators.push_back([this]() {
        return std::make_unique<PortalEvent>(this->d_audio);
   });
   d_eventGenerators.push_back([this]() {
        return std::make_unique<ExpandBoardEvent>(this->d_audio);
   });
   d_eventGenerators.push_back([this]() {
        return std::make_unique<SlipperyTerrainEvent>(this->d_audio);
   });
    //d_eventGenerators.push_back([this]() {
    //    return std::make_unique<FreezeEvent>(Position{-1, -1}, 3, this->d_audio);
    //});
    //d_eventGenerators.push_back([this]() {
    //    return std::make_unique<ObstacleEvent>(Position{-1, -1}, 4, this->d_audio);
    //});
        //d_eventGenerators.push_back([this]() {
       // return std::make_unique<FogEvent>(Position{-1, -1}, 4, this->d_audio);
  //  });
}

void EventManager::update() {
    if (d_messageTimer > 0.0f) d_messageTimer -= GetFrameTime();
}

bool EventManager::hasActiveMessage() const { return d_messageTimer > 0.0f; }
const char* EventManager::getActiveMessage() const { return d_currentMessage.c_str(); }

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
    // Vérifier si c'est l'événement d'expansion et qu'il ne s'est pas déjà produit
    if (auto expandEvent = dynamic_cast<ExpandBoardEvent*>(newEvent.get())) {
        if (d_boardHasExpanded) {
            return; // Ignorer si déjà étendu
        }
        d_boardHasExpanded = true; // Marquer comme étendu
    }

    newEvent->start(board); 
    
    d_currentMessage = newEvent->getMessage();
    d_messageTimer = 4.0f;

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