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
        Position movePieceWithPortal(Position from, Position to);

        Position getEnPassantTarget() const ;
        void setEnPassantTarget(Position target) ;

        Position getKingPosition(PieceColor color) const;
        bool isKingInCheck(PieceColor kingColor) const;

        PieceType getLastMovedPieceType() const { return d_lastMovedPieceType; }
        void setLastMovedPieceType(PieceType type) { d_lastMovedPieceType = type; }

        void setPortals(Position p1, Position p2);
        void clearPortals();
        bool isPortal(Position pos) const;
        Position getPortalExit(Position enter) const;

        void expandBoard();

        void setSlipperyTerrainActive(bool active, int turnsRemaining);
        void decrementSlipperyTurns();
        int getSlipperyTurnsRemaining() const { return d_slipperyTurnsRemaining; }
        bool isSlipperyTerrainActive() const { return d_isSlipperyTerrainActive; }

    private:
        std::vector<std::vector<Tile>> d_grid;
        int d_width;
        int d_height;
        Position d_enPassantTarget = Position::NONE;
        PieceType d_lastMovedPieceType = PieceType::Pawn;
        Position d_portal1 = {-1, -1};
        Position d_portal2 = {-1, -1};
        bool d_isSlipperyTerrainActive = false;
        int d_slipperyTurnsRemaining = 0;
};