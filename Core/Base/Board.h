#pragma once

#include <vector>
#include "Tile.h"

class Board{
    const static int BASE_WIDTH = 8;
    const static int BASE_HEIGHT = 8;
    public:
        Board(int width = BASE_WIDTH,int height =BASE_HEIGHT);

        int getWidth()const;
        int getHeight()const;


        bool isinBounds(Position pos)const;

        Tile& getTile(Position pos);
        const Tile& getTile(Position pos)const;

        void movePiece(Position from,Position to);

        Position getEnPassantTarget() const ;
        void setEnPassantTarget(Position target) ;

        Position getKingPosition(PieceColor color) const;
        bool isKingInCheck(PieceColor kingColor) const;

        PieceType getLastMovedPieceType() const { return d_lastMovedPieceType; }
        void setLastMovedPieceType(PieceType type) { d_lastMovedPieceType = type; }

    private:
        std::vector<std::vector<Tile>> d_grid;
        int d_width;
        int d_height;
        Position d_enPassantTarget = Position::NONE;
        PieceType d_lastMovedPieceType = PieceType::Pawn;
};