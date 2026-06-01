#pragma once
#include <functional>
#include <string>
#include <raylib.h>

class Button {
public:
    Button(float x, float y, const std::string& text);
    
    bool update(int mouseX, int mouseY, bool isMousePressed);
    void setOnClick(const std::function<void()>& callback);

    void draw() const;
    void setPosition(float x, float y);
    float getWidth() const;
    float getHeight() const;
    void setSize(float w, float h);
    void setText(const std::string& text);
    float getTextWidth() const;
    float getPreferredWidth() const;

private:
    static constexpr int FONT_SIZE = 20;
    static constexpr float PADDING_X = 20.0f;
    static constexpr float PADDING_Y = 10.0f;
    
    Rectangle d_bounds;
    std::string d_text;
    bool d_isHovered;
    std::function<void()> d_onClick;

    Color d_normalColor;
    Color d_hoverColor;
    Color d_textColor;
};