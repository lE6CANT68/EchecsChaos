#pragma once
#include "../Base/Board.h"
#include "../Base/Position.h"
#include "../../Graphics/UI/PromotionMenu.h"

class PromotionHandler {
public:
    static bool checkPromotion(const Board& board, Position targetPos);
    static bool handlePromotion(Board& board, const PromotionMenu& menu, Position promotionPos, int mouseX, int mouseY);
};