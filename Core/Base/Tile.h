#pragma once
#include <memory>
 #include "Piece.h" 

enum class TileType {
    Normal,
    Lava,
    Ice,
};

class Tile {

    public:
        Tile() : d_type(TileType::Normal) {}

        TileType getType() const { return d_type; }
        void setType(TileType newType) { d_type = newType; }

        bool hasPiece()const {return d_currentPiece !=nullptr;}

        Piece* getPiece()const{return d_currentPiece.get();}

        void setPiece(std::unique_ptr<Piece> p) { d_currentPiece = std::move(p); }

        std::unique_ptr<Piece> removePiece() { return std::move(d_currentPiece); }

        std::unique_ptr<Piece> releasePiece() {return std::move(d_currentPiece); }

        


    private:
        TileType d_type;
        std::unique_ptr<Piece> d_currentPiece; 
};