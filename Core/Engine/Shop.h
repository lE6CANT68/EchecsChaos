#pragma once
#include <vector>
#include <memory>
#include "../Base/Card.h"
#include "../Base/Player.h"

class Shop {
public:
    Shop() = default;

    void addCard(std::unique_ptr<Card> card);
    void refill(std::unique_ptr<Card> card);

    const std::vector<std::unique_ptr<Card>>& getCards() const { return d_cards; }

    static int getPrice(CardRarity rarity);

    std::unique_ptr<Card> buyCard(int index, Player& player);

private:
    std::vector<std::unique_ptr<Card>> d_cards;
};