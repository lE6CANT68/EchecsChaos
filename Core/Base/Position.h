#pragma once

struct Position {
    int x, y;

    bool operator==(const Position& p) const {
        return x == p.x && y == p.y;
    }
    
    bool operator!=(const Position& p) const {
        return x != p.x || y != p.y;
    }

    static const Position NONE;

    bool isValid() const {
        return x >= 0 && y >= 0; 
    }
};
inline constexpr Position Position::NONE = {-1, -1};