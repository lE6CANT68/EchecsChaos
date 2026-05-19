#include "PromotionHandler.h"
#include "../Pieces/Queen.h"
#include "../Pieces/Rook.h"
#include "../Pieces/Knight.h"
#include "../Pieces/Bishop.h"
#include "../Config/Constant.h"
#include <memory>

bool PromotionHandler::checkPromotion(const Board& board, Position targetPos) {
    Piece* piece = board.getTile(targetPos).getPiece();
    if (!piece) return false;

    if (piece->getType() == PieceType::Pawn) {
        if ((piece->getColor() == PieceColor::White && targetPos.y == Config::Board::PAWN_PROMOTION_WHITE_ROW) ||
            (piece->getColor() == PieceColor::Black && targetPos.y == Config::Board::PAWN_PROMOTION_BLACK_ROW)) {
            return true;
        }
    }
    return false;
}

bool PromotionHandler::handlePromotion(Board& board, const PromotionMenu& menu, Position promotionPos, int mouseX, int mouseY) {
    int startX = menu.getStartX();
    int startY = menu.getStartY();
    int boxSize = menu.getBoxSize();
    int menuWidth = menu.getOptions().size() * boxSize;
    
    if (mouseY < startY || mouseY > startY + boxSize || 
        mouseX < startX || mouseX > startX + menuWidth) {
        return false; 
    }

    int choice = (mouseX - startX) / boxSize;
    PieceColor color = board.getTile(promotionPos).getPiece()->getColor();

    if (choice == 0) board.getTile(promotionPos).setPiece(std::make_unique<Queen>(color));
    else if (choice == 1) board.getTile(promotionPos).setPiece(std::make_unique<Rook>(color));
    else if (choice == 2) board.getTile(promotionPos).setPiece(std::make_unique<Knight>(color));
    else if (choice == 3) board.getTile(promotionPos).setPiece(std::make_unique<Bishop>(color));

    return true; 
}