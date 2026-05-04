#pragma once
#include "../Core/Base/Board.h" 

enum class EffectType {
    MeteoriteWarning,
    BlackHoleWarning,
    IceTrap,
};

struct VisualEffect {
    Position pos;
    EffectType type;
};