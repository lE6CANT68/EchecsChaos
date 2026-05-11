#include "Shop.h"

int Shop::getPrice(CardRarity rarity) {
    switch(rarity) {
        case CardRarity::Common:    return 3;
        case CardRarity::Rare:      return 6;
        case CardRarity::Epic:      return 10;
        case CardRarity::Legendary: return 18;
        default:                    return 5;
    }
}

void Shop::addCard(std::unique_ptr<Card> card) {
    if (d_cards.size() < 5 && card != nullptr) {
        d_cards.push_back(std::move(card));
    }
}

void Shop::refill(std::unique_ptr<Card> card) {
    if (d_cards.size() < 5 && card != nullptr) {
        d_cards.push_back(std::move(card));
    }
}

std::unique_ptr<Card> Shop::buyCard(int index, Player& player) {
    if (index < 0 || index >= d_cards.size()) return nullptr;

    int price = getPrice(d_cards[index]->getRarity());
    if (player.getScore() >= price) {
        player.spendScore(price); 

        std::unique_ptr<Card> boughtCard = std::move(d_cards[index]);
        d_cards.erase(d_cards.begin() + index); 
        
        return boughtCard;
    }
    
    return nullptr; 
}