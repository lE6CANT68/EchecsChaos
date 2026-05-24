#include "Board.h"
#include <stdexcept>
#include <cctype>
#include "../Pieces/Pawn.h"
#include "../Pieces/Bishop.h"
#include "../Pieces/Knight.h"
#include "../Pieces/Rook.h"
#include "../Pieces/King.h"
#include "../Pieces/Queen.h"
#include "../Pieces/Chameleon.h"
#include "../Pieces/Duck.h"
#include "../Pieces/Idiot.h"
#include "../Pieces/PionDebile.h"

static char pieceTypeToFENLetter(PieceType type) {
    switch (type) {
        case PieceType::Pawn:      return 'P';
        case PieceType::Bishop:    return 'B';
        case PieceType::Knight:    return 'N';
        case PieceType::Rook:      return 'R';
        case PieceType::King:      return 'K';
        case PieceType::Queen:     return 'Q';
        case PieceType::Chameleon: return 'C';
        case PieceType::Duck:      return 'D';
        case PieceType::Idiot:     return 'I';
        case PieceType::PionDebile:return 'X';
        default:                   return '?';
    }
}

static std::string pieceToFENChar(const Piece* piece) {
    if (!piece) {
        return "";
    }
    char letter = pieceTypeToFENLetter(piece->getType());
    if (piece->getColor() != PieceColor::White) {
        letter = static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
    }
    return std::string(1, letter);
}

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

std::string Board::toString(PieceColor activeColor) const {
    std::ostringstream fen;
    for (int y = 0; y < d_height; ++y) {
        int emptyCount = 0;
        for (int x = 0; x < d_width; ++x) {
            const Tile& tile = getTile({x, y});
            if (!tile.hasPiece()) {
                emptyCount++;
                continue;
            }
            if (emptyCount > 0) {
                fen << emptyCount;
                emptyCount = 0;
            }
            fen << pieceToFENChar(tile.getPiece());
        }
        if (emptyCount > 0) {
            fen << emptyCount;
        }
        if (y < d_height - 1) {
            fen << '/';
        }
    }
    fen << ' ';
    fen << (activeColor == PieceColor::White ? 'w' : activeColor == PieceColor::Black ? 'b' : 'w');
    fen << " - - 0 1";
    return fen.str();
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
    std::unique_ptr<Piece> pieceToMove = getTile(from).releasePiece();
    if (!pieceToMove) return {-1, -1};

    Position finalDestination = to;
    if (isPortal(to)) {
        Position exit = getPortalExit(to);
        if (isinBounds(exit) && !getTile(exit).hasPiece()) {
            finalDestination = exit;
            clearPortals(); 
        }
    }
    getTile(finalDestination).setPiece(std::move(pieceToMove));
    if (getTile(finalDestination).hasPiece()) {
        getTile(finalDestination).getPiece()->setHasMoved(true);
    }
    return finalDestination;
}

void Board::movePiece(Position from, Position to) {
    movePieceWithPortal(from, to);
}

void Board::expandBoard() {
    int oldWidth = d_width;
    int oldHeight = d_height;
    d_width = 10;
    d_height = 10;
    std::vector<std::vector<Tile>> newGrid(d_width);
    for (int i = 0; i < d_width; ++i) {
        newGrid[i].resize(d_height);
    }
    for (int x = 0; x < oldWidth; ++x) {
        for (int y = 0; y < oldHeight; ++y) {
            if (d_grid[x][y].hasPiece()) {
                std::unique_ptr<Piece> piece = d_grid[x][y].releasePiece();
                newGrid[x + 1][y + 1].setPiece(std::move(piece));
            }
        }
    }
    d_grid = std::move(newGrid);
    if (d_portal1.x != -1) {
        d_portal1.x += 1;
        d_portal1.y += 1;
    }
    if (d_portal2.x != -1) {
        d_portal2.x += 1;
        d_portal2.y += 1;
    }
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

static std::string posToAlgebraic(int x, int y, int height) {
    char col = 'a' + x;
    std::string row = std::to_string(height - y);
    return std::string(1, col) + row;
}

std::string Board::getChaosStateString() const {
    std::ostringstream chaos;
    bool hasChaos = false;

    // 1. Terrain glissant global
    if (d_isSlipperyTerrainActive) {
        chaos << "- Le terrain est GLISSANT pour encore " << d_slipperyTurnsRemaining << " tour(s).\n";
        hasChaos = true;
    }

    // 2. Portails
    if (d_portal1.x != -1 && d_portal2.x != -1) {
        chaos << "- Un PORTAIL teleporte de " << posToAlgebraic(d_portal1.x, d_portal1.y, d_height) 
              << " a " << posToAlgebraic(d_portal2.x, d_portal2.y, d_height) << ".\n";
        hasChaos = true;
    }

    // 3. Effets specifiques aux cases
    for (int y = 0; y < d_height; ++y) {
        for (int x = 0; x < d_width; ++x) {
            const Tile& tile = getTile({x, y});
            std::string caseNom = posToAlgebraic(x, y, d_height);

            switch (tile.getType()) {
                case TileType::Lava:
                    chaos << "- La case " << caseNom << " est recouverte de LAVE (danger mortel).\n";
                    hasChaos = true;
                    break;
                case TileType::Ice:
                    chaos << "- La case " << caseNom << " est recouverte de GLACE (glissante).\n";
                    hasChaos = true;
                    break;
                case TileType::Frozen:
                    chaos << "- La case " << caseNom << " est GELEE (impossible a deplacer).\n";
                    hasChaos = true;
                    break;
                case TileType::Blocked:
                    chaos << "- La case " << caseNom << " est BLOQUEE (infranchissable).\n";
                    hasChaos = true;
                    break;
                default:
                    break;
            }

            // Brouillard
            if (tile.isFoggy()) {
                chaos << "- La case " << caseNom << " est couverte de BROUILLARD (vision limitee).\n";
                hasChaos = true;
            }

            // Glissade locale
            if (tile.isSlippery()) {
                chaos << "- La case " << caseNom << " est GLISSANTE (trajectoire modifiee).\n";
                hasChaos = true;
            }
        }
    }

    if (!hasChaos) {
        return "Aucun effet chaotique environnemental actif.\n";
    }

    return chaos.str();
}

void Board::loadFromString(const std::string& fen) {
    for(int x = 0; x < d_width; ++x) {
        for(int y = 0; y < d_height; ++y) {
            d_grid[x][y].setPiece(nullptr);
            d_grid[x][y].setType(TileType::Normal);
        }
    }
    int x = 0, y = 0;
    for(char c : fen) {
        if (c == ' ') break;
        if (c == '/') {
            y++;
            x = 0;
        } else if (std::isdigit(c)) {
            x += (c - '0');
        } else {
            PieceColor color = std::islower(c) ? PieceColor::Black : PieceColor::White;
            char upperC = std::toupper(c);
            std::unique_ptr<Piece> p;
            switch(upperC) {
                case 'P': p = std::make_unique<Pawn>(color); break;
                case 'B': p = std::make_unique<Bishop>(color); break;
                case 'N': p = std::make_unique<Knight>(color); break;
                case 'R': p = std::make_unique<Rook>(color); break;
                case 'K': p = std::make_unique<King>(color); break;
                case 'Q': p = std::make_unique<Queen>(color); break;
                case 'C': p = std::make_unique<Chameleon>(color); break;
                case 'D': p = std::make_unique<Duck>(color); break;
                case 'I': p = std::make_unique<Idiot>(color); break;
                case 'X': p = std::make_unique<PionDebile>(color); break;
            }
            if (p && isinBounds({x, y})) {
                d_grid[x][y].setPiece(std::move(p));
            }
            x++;
        }
    }
}
