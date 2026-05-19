#pragma once
#include "Button.h"
#include <functional>
#include <raylib.h>
#include "../../Core/Config/Constant.h"

class GamemodeMenu {
public:
    enum class Selection {
        None,
        Local,
        AI,
        Online,
        Back
    };

    GamemodeMenu();

    void update(int mouseX, int mouseY, bool isMousePressed);
    void draw() const;
    void setButtonClickCallback(const std::function<void()>& callback);

    Selection getSelection() const;
    void clearSelection();

private:
    Button d_localButton;
    Button d_aiButton;
    Button d_onlineButton;
    Button d_backButton;
    Selection d_selection;
    std::function<void()> d_buttonClickCallback;
};
