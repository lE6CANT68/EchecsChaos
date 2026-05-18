#pragma once
#include "Button.h"
#include <raylib.h>
#include "../../Core/Config/Constant.h"

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

    Action getSelectedAction() const;
    void clearAction();

private:
    Button d_startButton;
    Button d_settingsButton;
    Button d_quitButton;
    Action d_selectedAction;
};
