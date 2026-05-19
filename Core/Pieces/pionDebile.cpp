#include "PionDebile.h"
#include "../Base/Board.h"

PionDebile::PionDebile(PieceColor c) : Piece(c, PieceType::PionDebile) {}

std::vector<Position> PionDebile::getValidMoves(Position currentPos, const Board& board) const {
    std::vector<Position> moves;
    moves.reserve(MAX_MOVES);

    // Direction selon la couleur : les Blancs montent (-1), les Noirs descendent (+1)
    int dir = (getColor() == PieceColor::White) ? -1 : 1;

    // --- 1. AVANCER (Sans manger) : Uniquement en diagonale ---
    Position diagLeft = { currentPos.x - 1, currentPos.y + dir };
    if (board.isinBounds(diagLeft) && board.getTile(diagLeft).isWalkable() && !board.getTile(diagLeft).hasPiece()) {
        moves.push_back(diagLeft);
    }

    Position diagRight = { currentPos.x + 1, currentPos.y + dir };
    if (board.isinBounds(diagRight) && board.getTile(diagRight).isWalkable() && !board.getTile(diagRight).hasPiece()) {
        moves.push_back(diagRight);
    }

    // --- 2. MANGER : Uniquement tout droit ---
    Position straight = { currentPos.x, currentPos.y + dir };
    if (board.isinBounds(straight) && board.getTile(straight).isWalkable() && board.getTile(straight).hasPiece()) {
        // S'il y a une pièce, on vérifie qu'elle appartient bien à l'adversaire
        if (board.getTile(straight).getPiece()->getColor() != this->getColor()) {
            moves.push_back(straight);
        }
    }

    return moves;
}