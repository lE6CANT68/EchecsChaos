#include "Button.h"

Button::Button(float x, float y, const std::string& text)
    : d_text(text), d_isHovered(false)
{
    int textWidth = MeasureText(text.c_str(), FONT_SIZE);
    float width = textWidth + (PADDING_X * 2);
    float height = FONT_SIZE + (PADDING_Y * 2);
    
    d_bounds = {x, y, width, height};

    d_normalColor = { 80, 70, 100, 255 }; // Grey violet
    d_hoverColor = { 50, 45, 70, 255 }; // Darker on hover
    d_textColor = { 255, 190, 20, 255 }; // Gold
}

void Button::setOnClick(const std::function<void()>& callback) {
    d_onClick = callback;
}

bool Button::update(int mouseX, int mouseY, bool isMousePressed) {
    Vector2 mousePos = { (float)mouseX, (float)mouseY };

    d_isHovered = CheckCollisionPointRec(mousePos, d_bounds);

    if (d_isHovered && isMousePressed) {
        if (d_onClick) {
            d_onClick();
        }
        return true; 
    }
    return false;
}

void Button::draw() const {
    Color bgColor = d_isHovered ? d_hoverColor : d_normalColor;

    DrawRectangleRec(d_bounds, bgColor);
    DrawRectangleLinesEx(d_bounds, 2, d_textColor);

    int textWidth = MeasureText(d_text.c_str(), FONT_SIZE);
    
    int textX = d_bounds.x + (d_bounds.width - textWidth) / 2.0f;
    int textY = d_bounds.y + (d_bounds.height - FONT_SIZE) / 2.0f;

    DrawText(d_text.c_str(), textX, textY, FONT_SIZE, d_textColor);
}

void Button::setPosition(float x, float y) {
    d_bounds.x = x;
    d_bounds.y = y;
}

float Button::getWidth() const {
    return d_bounds.width;
}

float Button::getHeight() const {
    return d_bounds.height;
}

void Button::setSize(float w, float h) {
    d_bounds.width = w;
    d_bounds.height = h;
}

void Button::setText(const std::string& text) {
    d_text = text;
    int textWidth = MeasureText(d_text.c_str(), FONT_SIZE);
    float width = textWidth + (PADDING_X * 2);
    float height = FONT_SIZE + (PADDING_Y * 2);
    d_bounds.width = width;
    d_bounds.height = height;
}

float Button::getTextWidth() const {
    return (float)MeasureText(d_text.c_str(), FONT_SIZE);
}

float Button::getPreferredWidth() const {
    return getTextWidth() + (PADDING_X * 2);
}
