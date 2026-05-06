#include "Board.h"
#include <stdexcept>


Board::Board(int width, int height):d_width(width),d_height(height){
    d_grid.resize(width);

    for(int i = 0;i <width ;++i){
        d_grid[i].resize(height);
    }
}

int Board::getWidth()const{
    return d_width;
}

int Board::getHeight()const{
    return d_height;
}


bool Board::isinBounds(Position pos)const{
    return pos.x >= 0 && pos.y >= 0 &&pos.x < d_width && pos.y < d_height; 
}

Tile& Board::getTile(Position pos){
    if(!isinBounds(pos)){
        throw std::out_of_range("Case en dehors du tableau");
    }
    return d_grid[pos.x][pos.y];
}

const Tile&  Board::getTile(Position pos)const{
    if(!isinBounds(pos)){
        throw std::out_of_range("Case en dehors du tableau");
    }
    return d_grid[pos.x][pos.y];
}

void Board::movePiece(Position from,Position to){
    if(isinBounds(from) &&isinBounds(to)){
        std::unique_ptr<Piece> pieceChoisie = getTile(from).removePiece();

        getTile(to).setPiece(std::move(pieceChoisie));
        getTile(to).getPiece()->setHasMoved(true);
    }
}
Position Board::getEnPassantTarget() const { 
    return d_enPassantTarget; 
}
void Board::setEnPassantTarget(Position target) { 
    d_enPassantTarget = target;
 }
Position Board::getKingPosition(PieceColor color) const {
    for (int x = 0; x < d_width; ++x) {
        for (int y = 0; y < d_height; ++y) {
            const Tile& tile = getTile({x, y});
            if (tile.hasPiece() && 
                tile.getPiece()->getType() == PieceType::King && 
                tile.getPiece()->getColor() == color) {
                return {x, y};
            }
        }
    }
    throw std::runtime_error("CRASH MOTEUR : Le Roi est introuvable sur le plateau !");
}
bool Board::isKingInCheck(PieceColor kingColor) const {
    Position kingPos = getKingPosition(kingColor);
    for (int x = 0; x < d_width; ++x) {
        for (int y = 0; y < d_height; ++y) {
            const Tile& tile = getTile({x, y});
            if (tile.hasPiece() && tile.getPiece()->getColor() != kingColor) {
                std::vector<Position> enemyMoves = tile.getPiece()->getValidMoves({x, y}, *this);
                
                for (const Position& move : enemyMoves) {
                    if (move == kingPos) {
                        return true; 
                    }
                }
            }
        }
    }
    return false;
}