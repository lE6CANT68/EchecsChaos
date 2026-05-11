#include "PromotionMenu.h"


PromotionMenu::PromotionMenu(const std::vector<PieceType>& options) 
    : d_options(options), 
      d_boxSize(Config::Graphics::PROMOTION_BOX_SIZE), 
      d_startY(Config::Graphics::PROMOTION_MENU_Y) 
{
    int menuWidth = (int)d_options.size() * d_boxSize;
    int boardStartX = (int)Config::Graphics::DEFAULT_OFFSETX;
    int boardWidth = 8 * (int)Config::Graphics::CELL_SIZE;
    d_startX = boardStartX + (boardWidth - menuWidth) / 2;
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