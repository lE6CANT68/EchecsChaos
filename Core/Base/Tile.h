#pragma once
#include <memory>
#include "Piece.h" 

enum class TileType {
    Normal,
    Lava,
    Ice,
    Blocked,
    Frozen,
};

class Tile {
    public:
        // Ajout de d_isSlippery{false} dans l'initialisation
        Tile() : d_type(TileType::Normal), d_isFoggy{false}, d_isSlippery{false} {}

        TileType getType() const { return d_type; }
        void setType(TileType newType) { d_type = newType; }

        bool hasPiece() const { return d_currentPiece != nullptr; }
        Piece* getPiece() const { return d_currentPiece.get(); }

        void setPiece(std::unique_ptr<Piece> p) { d_currentPiece = std::move(p); }

        std::unique_ptr<Piece> removePiece() { return std::move(d_currentPiece); }

        std::unique_ptr<Piece> releasePiece() { return std::move(d_currentPiece); }

        bool isWalkable() const { return d_type != TileType::Blocked && d_type != TileType::Frozen; }
        bool isFoggy() const { return d_isFoggy; }
        void setFoggy(bool fog) { d_isFoggy = fog; }
        
        // --- NOUVELLES MÉTHODES POUR LA GLISSADE ---
        bool isSlippery() const { return d_isSlippery; }
        void setSlippery(bool slippery) { d_isSlippery = slippery; }

    private:
        TileType d_type;
        std::unique_ptr<Piece> d_currentPiece; 
        bool d_isFoggy;
        bool d_isSlippery; 
};