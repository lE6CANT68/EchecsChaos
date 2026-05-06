#pragma once
#include <vector>
#include <optional>
#include "../Base/Piece.h" 
#include "../Core/Config/Constant.h"

class PromotionMenu {
public:
    PromotionMenu(const std::vector<PieceType>& options);
    std::optional<PieceType> getClickedPiece(int mouseX, int mouseY) const;

    const std::vector<PieceType>& getOptions() const ;
    int getBoxSize() const ;
    int getStartX() const ;
    int getStartY() const ;

private:
    std::vector<PieceType> d_options;
    int d_boxSize;
    int d_startX;
    int d_startY;
};