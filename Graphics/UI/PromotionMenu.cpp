#include "PromotionMenu.h"

PromotionMenu::PromotionMenu(const std::vector<PieceType>& options) 
    : d_options(options), d_boxSize(100), d_startY(350) 
{
    d_startX = (800 - (d_options.size() * d_boxSize)) / 2;
}

std::optional<PieceType> PromotionMenu::getClickedPiece(int mouseX, int mouseY) const {
    if (mouseY < d_startY || mouseY > d_startY + d_boxSize || 
        mouseX < d_startX || mouseX > d_startX + (d_options.size() * d_boxSize)) {
        return std::nullopt; 
    }
    int choiceIndex = (mouseX - d_startX) / d_boxSize;
    return d_options[choiceIndex];
}
const std::vector<PieceType>& PromotionMenu::getOptions() const { return d_options; }
int PromotionMenu::getBoxSize() const { return d_boxSize; }
int PromotionMenu::getStartX() const { return d_startX; }
int PromotionMenu::getStartY() const { return d_startY; }