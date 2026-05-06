#include "BoardInteractionManager.h"
#include "SpecialMoveHandler.h"
#include "MoveValidator.h"

bool BoardInteractionManager::tryExecuteMove(Board& board, Position clickedPos, Position& selectedTile, const std::vector<Position>& validMoves, AudioManager& audioManager) {
    if (!selectedTile.isValid()) return false;

    for (const Position& validMove : validMoves) {
        if (clickedPos == validMove) { 
            bool isCapture = board.getTile(clickedPos).hasPiece();

            SpecialMoveHandler::handleCastling(board, selectedTile, clickedPos);
            SpecialMoveHandler::handleEnPassant(board, selectedTile, clickedPos);
            SpecialMoveHandler::updateEnPassantTarget(board, selectedTile, clickedPos);

            board.movePiece(selectedTile, clickedPos);
            board.getTile(clickedPos).getPiece()->setHasMoved(true);

            if (isCapture) audioManager.playCapture();
            else audioManager.playMove();

            selectedTile = Position::NONE;
            return true; 
        }
    }
    return false; 
}

void BoardInteractionManager::updateSelection(Board& board, Position clickedPos, PieceColor currentColor, Position& selectedTile, std::vector<Position>& validMoves) {
    const Tile& tile = board.getTile(clickedPos);
    
    if (tile.hasPiece() && tile.getPiece()->getColor() == currentColor) {
        selectedTile = clickedPos;
        std::vector<Position> pseudoMoves = tile.getPiece()->getValidMoves(clickedPos, board);
        validMoves = MoveValidator::filterLegalMoves(board, clickedPos, pseudoMoves);
    } else {
        selectedTile = Position::NONE;
        validMoves.clear();
    }
}