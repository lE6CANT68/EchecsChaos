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
    return {-1, -1}; 
}
bool Board::isKingInCheck(PieceColor kingColor) const {
    Position kingPos = getKingPosition(kingColor);
    if (kingPos.x == -1) return false; 

    for (int x = 0; x < d_width; ++x) {
        for (int y = 0; y < d_height; ++y) {
            const Tile& tile = getTile({x, y});
            if (tile.hasPiece() && tile.getPiece()->getColor() != kingColor) {
                std::vector<Position> enemyMoves = tile.getPiece()->getValidMoves({x, y}, *this);
                
                for (const Position& move : enemyMoves) {
                    if (move.x == kingPos.x && move.y == kingPos.y) {
                        return true; 
                    }
                }
            }
        }
    }
    return false;
}

void Board::setPortals(Position p1, Position p2) {
    d_portal1 = p1;
    d_portal2 = p2;
}

void Board::clearPortals() {
    d_portal1 = {-1, -1};
    d_portal2 = {-1, -1};
}

bool Board::isPortal(Position pos) const {
    return (pos.x == d_portal1.x && pos.y == d_portal1.y) || 
           (pos.x == d_portal2.x && pos.y == d_portal2.y);
}

Position Board::getPortalExit(Position enter) const {
    if (enter.x == d_portal1.x && enter.y == d_portal1.y) return d_portal2;
    if (enter.x == d_portal2.x && enter.y == d_portal2.y) return d_portal1;
    return {-1, -1};
}

Position Board::movePieceWithPortal(Position from, Position to) {
    if (!isinBounds(from) || !isinBounds(to)) return {-1, -1};

    // 1. Sécurité : On récupère la pièce
    std::unique_ptr<Piece> pieceToMove = getTile(from).releasePiece();
    if (!pieceToMove) return {-1, -1};

    Position finalDestination = to;

    // 2. Logique du Portail
    if (isPortal(to)) {
        Position exit = getPortalExit(to);
        // On vérifie si la sortie est libre
        if (isinBounds(exit) && !getTile(exit).hasPiece()) {
            finalDestination = exit;
            // On supprime les portails immédiatement après usage
            clearPortals(); 
        }
    }

    // 3. Placement final de la pièce
    getTile(finalDestination).setPiece(std::move(pieceToMove));
    
    // 4. On marque le mouvement pour le premier coup (pions/roque)
    if (getTile(finalDestination).hasPiece()) {
        getTile(finalDestination).getPiece()->setHasMoved(true);
    }
    
    // 5. On retourne la position réelle où la pièce a été placée
    return finalDestination;
}

void Board::movePiece(Position from, Position to) {
    movePieceWithPortal(from, to);
}

void Board::expandBoard() {
    // Sauvegarder les dimensions anciennes
    int oldWidth = d_width;
    int oldHeight = d_height;
    
    // Nouvelle taille : 10x10 (enveloppe complète autour du plateau 8x8)
    d_width = 10;
    d_height = 10;
    
    // Créer une nouvelle grille vide
    std::vector<std::vector<Tile>> newGrid(d_width);
    for (int i = 0; i < d_width; ++i) {
        newGrid[i].resize(d_height);
    }
    
    // Copier les pièces de l'ancienne grille vers la nouvelle avec décalage de 1 (crée l'enveloppe)
    for (int x = 0; x < oldWidth; ++x) {
        for (int y = 0; y < oldHeight; ++y) {
            if (d_grid[x][y].hasPiece()) {
                std::unique_ptr<Piece> piece = d_grid[x][y].releasePiece();
                newGrid[x + 1][y + 1].setPiece(std::move(piece));
            }
        }
    }
    
    // Remplacer la grille
    d_grid = std::move(newGrid);
    
    // Mettre à jour les portails s'ils existent
    if (d_portal1.x != -1) {
        d_portal1.x += 1;
        d_portal1.y += 1;
    }
    if (d_portal2.x != -1) {
        d_portal2.x += 1;
        d_portal2.y += 1;
    }
    
    // Mettre à jour la cible d'en passant
    if (d_enPassantTarget.x != -1) {
        d_enPassantTarget.x += 1;
        d_enPassantTarget.y += 1;
    }
}

void Board::setSlipperyTerrainActive(bool active, int turnsRemaining) {
    d_isSlipperyTerrainActive = active;
    d_slipperyTurnsRemaining = turnsRemaining;
}

void Board::decrementSlipperyTurns() {
    if (d_isSlipperyTerrainActive && d_slipperyTurnsRemaining > 0) {
        d_slipperyTurnsRemaining--;
        if (d_slipperyTurnsRemaining == 0) {
            d_isSlipperyTerrainActive = false;
        }
    }
}