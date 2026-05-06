#pragma once
#include "Position.h"
#include <vector>

class Board;

enum class PieceColor{
    White,
    Black,
};
enum class PieceType{
    Pawn,
    Bishop,
    Knight,
    Rook,
    King,
    Queen
};

class Piece{
    public:
        Piece(PieceColor c,PieceType t) :d_color(c),d_type(t),d_hasMoved(false){}
        virtual ~Piece() = default;

        PieceColor getColor()const{return d_color;}
        void setColor(PieceColor newColor){d_color = newColor;}

        PieceType getType()const{return d_type;}

        bool hasMoved() const { return d_hasMoved; }
        void setHasMoved(bool moved) { d_hasMoved = moved; }



       virtual std::vector<Position> getValidMoves(Position currentPos,const Board& board)const = 0;
    private:
        PieceColor d_color;
        PieceType d_type;
        bool d_hasMoved;
        
};