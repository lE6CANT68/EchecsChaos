#include "SpecialMoveHandler.h"
#include <cmath> 
#include <iostream>

void SpecialMoveHandler::handleCastling(Board& board, Position startPos, Position targetPos) {
    Piece* pieceToMove = board.getTile(startPos).getPiece();

  

    if (pieceToMove->getType() != PieceType::King || abs(targetPos.x - startPos.x) != Config::Board::CASTLING_DISTANCE) {
        return; 
    }
    const int y = targetPos.y; 
    
    if (targetPos.x == Config::Board::KING_CASTLE_TARGET_X) { 
        const int rookStartX = Config::Board::KING_ROOK_START_X;
        const int rookEndX = Config::Board::KING_ROOK_END_X;

        board.movePiece({rookStartX, y}, {rookEndX, y});
        board.getTile({rookEndX, y}).getPiece()->setHasMoved(true);
    } 
    else if (targetPos.x == Config::Board::QUEEN_CASTLE_TARGET_X) { 
        const int rookStartX = Config::Board::QUEEN_ROOK_START_X;
        const int rookEndX = Config::Board::QUEEN_ROOK_END_X;

        board.movePiece({rookStartX, y}, {rookEndX, y});
        board.getTile({rookEndX, y}).getPiece()->setHasMoved(true);
    }
}

void SpecialMoveHandler::handleEnPassant(Board& board, Position startPos, Position targetPos) {
    Piece* pieceToMove = board.getTile(startPos).getPiece();

    if (pieceToMove->getType() != PieceType::Pawn || targetPos != board.getEnPassantTarget()) {
        return;
    }

    Position enemyPawnPos = { targetPos.x, startPos.y };
    board.getTile(enemyPawnPos).setPiece(nullptr); 
}

void SpecialMoveHandler::updateEnPassantTarget(Board& board, Position startPos, Position targetPos) {
    Piece* pieceToMove = board.getTile(startPos).getPiece();

    if (pieceToMove->getType() == PieceType::Pawn && abs(targetPos.y - startPos.y) == 2) {
        int skippedY = (startPos.y + targetPos.y) / 2;
        board.setEnPassantTarget({ startPos.x, skippedY });
    } 
    else {
        board.setEnPassantTarget(Position::NONE);
    }
}