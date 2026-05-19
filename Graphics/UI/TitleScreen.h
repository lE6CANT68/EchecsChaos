#pragma once
#include "Button.h"
#include <functional>
#include <raylib.h>
#include "../../Core/Config/Constant.h"
#include "GamemodeMenu.h"

class TitleScreen {
public:
    enum class Action {
        None,
        Play,
        Settings,
        Quit
    };

    TitleScreen();

    void update(int mouseX, int mouseY, bool isMousePressed);
    void draw() const;
    void setButtonClickCallback(const std::function<void()>& callback);

    Action getSelectedAction() const;
    void clearAction();

private:
    Button d_startButton;
    Button d_settingsButton;
    Button d_quitButton;
    Action d_selectedAction;
    GamemodeMenu d_gamemodeMenu;
    bool d_showGamemode = false;
    std::function<void()> d_buttonClickCallback;
};
