#pragma once
#include <string>
#include <raylib.h>

class Button {
public:
    Button(float x, float y, const std::string& text);
    
    bool update(int mouseX, int mouseY, bool isMousePressed);

    void draw() const;

private:
    static constexpr int FONT_SIZE = 20;
    static constexpr float PADDING_X = 20.0f;
    static constexpr float PADDING_Y = 10.0f;
    
    Rectangle d_bounds;
    std::string d_text;
    bool d_isHovered;

    Color d_normalColor;
    Color d_hoverColor;
    Color d_textColor;
};