#pragma once
#include "Card.h"
#include "../Core/Event/Event.h"

class MysteryCard : public Card {
private:
    EventRarity d_eventRarity;

public:
    MysteryCard(EventRarity rarity) : d_eventRarity(rarity) {}

    std::string getName() const override { 
        switch (d_eventRarity) {
            case EventRarity::Common:    return "Mystere Commun";
            case EventRarity::Rare:      return "Mystere Rare";
            case EventRarity::Epic:      return "Mystere Epique";
            case EventRarity::Legendary: return "Mystere Legendaire";
            default:                     return "Carte Mystere";
        }
    }
    
    std::string getDescription() const override { 
        switch (d_eventRarity) {
            case EventRarity::Common:    return "Declenche un petit evenement aleatoire.";
            case EventRarity::Rare:      return "Declenche un evenement aleatoire perturbant.";
            case EventRarity::Epic:      return "Declenche un evenement aleatoire puissant.";
            case EventRarity::Legendary: return "Declenche un evenement aleatoire chaotique et cataclysmique !";
            default:                     return "Declenche un evenement aleatoire.";
        }
    }
    
    int getCost() const override { 
        switch (d_eventRarity) {
            case EventRarity::Common:    return 2;
            case EventRarity::Rare:      return 4;
            case EventRarity::Epic:      return 6;
            case EventRarity::Legendary: return 8;
            default:                     return 4; 
        }
    } 
    
    CardRarity getRarity() const override { 
        switch (d_eventRarity) {
            case EventRarity::Common:    return CardRarity::Common;
            case EventRarity::Rare:      return CardRarity::Rare;
            case EventRarity::Epic:      return CardRarity::Epic;
            case EventRarity::Legendary: return CardRarity::Legendary;
            default:                     return CardRarity::Common;
        }
    } 
    
    bool requiresTarget() const override { return false; } 

    void play(Player& player, Board& board, EventManager& eventManager, Position target) override {
        auto randomEvent = eventManager.generateRandomEvent(d_eventRarity);
        
        if (randomEvent) {
            eventManager.addEvent(board, std::move(randomEvent));
        }
    }
};