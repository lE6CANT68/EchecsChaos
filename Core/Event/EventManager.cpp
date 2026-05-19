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
     d_eventGenerators.push_back([this]() {
         return std::make_unique<ChameleonEvent>(this->d_audio);
     });
     d_eventGenerators.push_back([this]() {
         return std::make_unique<PionDebileEvent>(this->d_audio);
     });
     d_eventGenerators.push_back([this]() {
         return std::make_unique<LavaWallEvent>(generateRandomWall(), 5, this->d_audio);
     });
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

    int triggerChance = GetRandomValue(1, 100);
    if (triggerChance > d_eventProbability) {
        return; 
    }
    int rarityRoll = GetRandomValue(1, 100);
    EventRarity selectedRarity;

    if (rarityRoll <= 50) {
        selectedRarity = EventRarity::Common;      // 50% 
    } 
    else if (rarityRoll <= 80) {
        selectedRarity = EventRarity::Rare;        // 30% 
    } 
    else if (rarityRoll <= 95) {
        selectedRarity = EventRarity::Epic;        // 15% 
    } 
    else {
        selectedRarity = EventRarity::Legendary;   // 5% 
    }
    std::unique_ptr<Event> newEvent = generateRandomEvent(selectedRarity);
    
    if (newEvent) {
        addEvent(board, std::move(newEvent));
    }
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

std::vector<Position> EventManager::generateRandomWall() {
    std::vector<Position> wallPositions;
    
    // Génère une ligne ou une zone de lave aléatoire
    int wallType = GetRandomValue(0, 2);
    
    if (wallType == 0) {
        // Mur horizontal
        int y = GetRandomValue(2, 5);
        int startX = GetRandomValue(0, 3);
        int length = GetRandomValue(2, 4);
        for (int x = startX; x < startX + length && x < 8; ++x) {
            wallPositions.push_back({x, y});
        }
    } else if (wallType == 1) {
        // Mur vertical
        int x = GetRandomValue(2, 5);
        int startY = GetRandomValue(0, 3);
        int length = GetRandomValue(2, 4);
        for (int y = startY; y < startY + length && y < 8; ++y) {
            wallPositions.push_back({x, y});
        }
    } else {
        // Zone en L
        int x = GetRandomValue(2, 5);
        int y = GetRandomValue(2, 5);
        for (int i = 0; i < 3; ++i) {
            if (x + i < 8) wallPositions.push_back({x + i, y});
            if (y + i < 8) wallPositions.push_back({x, y + i});
        }
    }
    
    return wallPositions;
}

std::unique_ptr<Event> EventManager::generateRandomEvent(EventRarity rarity) {
    switch (rarity) {
        case EventRarity::Common: {
            int roll = GetRandomValue(0, 3);
            switch (roll) {
                case 0:
                    return std::make_unique<FreezeEvent>(Position::NONE, Config::Event::FREEZE_EVENT_DURATION, d_audio);
                case 1:
                    return std::make_unique<ObstacleEvent>(Position::NONE, Config::Event::OBSTACLE_EVENT_DURATION, d_audio);
                case 2:
                    return std::make_unique<FogEvent>(Position::NONE, Config::Event::FOG_EVENT_DURATION, d_audio);
                case 3:
                    return std::make_unique<PionDebileEvent>(d_audio);
                default:
                    return std::make_unique<FreezeEvent>(Position::NONE, Config::Event::FREEZE_EVENT_DURATION, d_audio);
            }
        }
        case EventRarity::Rare: {
            int roll = GetRandomValue(0, 3);
            switch (roll) {
                case 0:
                    return std::make_unique<MeteoriteEvent>(d_audio);
                case 1:
                    return std::make_unique<IdiotEvent>(d_audio);
                case 2:
                    return std::make_unique<PortalEvent>(d_audio);
                case 3:
                    return std::make_unique<SlipperyTerrainEvent>(d_audio);
                default:
                    return std::make_unique<MeteoriteEvent>(d_audio);
            }
        }
        case EventRarity::Epic: {
            int roll = GetRandomValue(0, 2);
            switch (roll) {
                case 0: return std::make_unique<HideTimeEvent>(PlayerTarget::Both, Config::Event::HIDE_TIME_EVENT_DURATION, d_audio);
                case 1: return std::make_unique<DuckEvent>(d_audio);
                case 2: return std::make_unique<ExpandBoardEvent>(d_audio);
                default: return std::make_unique<HideTimeEvent>(PlayerTarget::Both, Config::Event::HIDE_TIME_EVENT_DURATION, d_audio);
            }
        }
        case EventRarity::Legendary: {
            int roll = GetRandomValue(0, 1);
            switch (roll) {
                case 0:
                    return std::make_unique<ChameleonEvent>(d_audio);
                case 1:
                    return std::make_unique<LavaWallEvent>(generateRandomWall(), 5, d_audio);
                default:
                    return std::make_unique<ChameleonEvent>(d_audio);
            }
        }
        default:
            return std::make_unique<FreezeEvent>(Position::NONE, Config::Event::FREEZE_EVENT_DURATION, d_audio);
    }
}
