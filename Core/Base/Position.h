#pragma once

struct Position{
    int x,y;

    bool operator==(const Position& p)const{
        return x==p.x && y==p.y;
    }
    bool operator!=(const Position& p)const{
        return x!=p.x || y!=p.y;
    }
};