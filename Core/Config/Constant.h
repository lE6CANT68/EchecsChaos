#pragma once
#include <raylib.h>

namespace Config{
    namespace Graphics{
        constexpr int CONFIG_WINDOW_WIDTH = 1000;
        constexpr int CONFIG_WINDOW_HEIGHT = 1000;
        constexpr int CONFIG_TARGET_FPS = 60;

        constexpr float CELL_SIZE = 80.0f;
        constexpr float DEFAULT_OFFSETX = 200.0f;
        constexpr float DEFAULT_OFFSETY = 200.0f;

        constexpr int  PROMOTION_MENU_WIDTH = 1000;
        constexpr int  PROMOTION_MENU_HEIGHT = 1000;
        constexpr int  PROMOTION_BOX_SIZE = 100;
        constexpr int  PROMOTION_MENU_Y = 350;

        constexpr float MESSAGE_DISPLAY_TIME = 4.0f;

    }
    namespace Colors{
        const Color HIGHLIGHT_COLOR = {46, 204, 113, 150};
        const Color HINT_COLOR = {46,204,113,150};
        const Color FOG_COLOR = {15, 15, 15, 255};
        const Color CHECK_HIGHLIGHT_COLOR = { 220, 20, 20, 180 };
    }
    namespace Board {
        constexpr int WIDTH = 8;
        constexpr int HEIGHT = 8;

        constexpr int BLACK_BACK_ROW = 0;
        constexpr int BLACK_PAWN_ROW = 1;
        constexpr int WHITE_PAWN_ROW = 6;
        constexpr int WHITE_BACK_ROW = 7;

        constexpr int CASTLING_DISTANCE = 2;

        constexpr int KING_CASTLE_TARGET_X = 6;
        constexpr int QUEEN_CASTLE_TARGET_X = 2;
        
        constexpr int KING_ROOK_START_X = 7;
        constexpr int KING_ROOK_END_X = 5;

        
        constexpr int QUEEN_ROOK_START_X = 0;
        constexpr int QUEEN_ROOK_END_X = 3;

        constexpr int PAWN_PROMOTION_WHITE_ROW = 0;
        constexpr int PAWN_PROMOTION_BLACK_ROW = 7;

    }
    namespace Audio{
        constexpr float  DEFAULT_MUSIC_VOLUME = 0.4f;
    }
    namespace Event{
        constexpr int FREEZE_EVENT_DURATION = 3;
        constexpr int OBSTACLE_EVENT_DURATION = 4;
        constexpr int FOG_EVENT_DURATION = 4;
        constexpr int HIDE_TIME_EVENT_DURATION = 3;

    }
}