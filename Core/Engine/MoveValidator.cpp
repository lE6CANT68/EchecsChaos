#include "MoveValidator.h"
#include <cmath>
#include <memory>

std::vector<Position> MoveValidator::filterLegalMoves(Board& board, Position startPos, const std::vector<Position>& pseudoMoves) {
    std::vector<Position> legalMoves;
    Piece* myPiece = board.getTile(startPos).getPiece();
    PieceColor myColor = myPiece->getColor();

    bool isKing = (myPiece->getType() == PieceType::King);
    bool currentlyInCheck = board.isKingInCheck(myColor);

    for (const Position& targetPos : pseudoMoves) {
        if (isKing && abs(targetPos.x - startPos.x) == 2) {
            if (currentlyInCheck) continue; 
            int step = (targetPos.x > startPos.x) ? 1 : -1; 
            Position crossedPos = { startPos.x + step, startPos.y };

            std::unique_ptr<Piece> backupCrossed = board.getTile(crossedPos).releasePiece();
            std::unique_ptr<Piece> kingToMove = board.getTile(startPos).releasePiece();
            
            board.getTile(crossedPos).setPiece(std::move(kingToMove));
            bool isCrossedSafe = !board.isKingInCheck(myColor);
            kingToMove = board.getTile(crossedPos).releasePiece();
            board.getTile(startPos).setPiece(std::move(kingToMove));
            if (backupCrossed) board.getTile(crossedPos).setPiece(std::move(backupCrossed));
            
            if (!isCrossedSafe) continue; 
        }
        std::unique_ptr<Piece> backupTarget = board.getTile(targetPos).releasePiece(); 
        std::unique_ptr<Piece> pieceToMove = board.getTile(startPos).releasePiece();
        board.getTile(targetPos).setPiece(std::move(pieceToMove));
        bool isSafe = !board.isKingInCheck(myColor);
        std::unique_ptr<Piece> pieceToReturn = board.getTile(targetPos).releasePiece();
        board.getTile(startPos).setPiece(std::move(pieceToReturn));

        if (backupTarget) {
            board.getTile(targetPos).setPiece(std::move(backupTarget));
        }
        if (isSafe) {
            legalMoves.push_back(targetPos); 
        }
    }

    return legalMoves;
}