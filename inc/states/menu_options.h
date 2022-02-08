#ifndef MENU_OPTIONS_STATE_H
#define MENU_OPTIONS_STATE_H

#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"

class OptionsMenu : public GameState {
public:
    void            Init(Engine* e);
    void            Cleanup(Engine* e);
    void            Pause(Engine* e);
    void            Resume(Engine* e);
    void            HandleEvents(Engine* e);
    void            Update(Engine* e);
    void            Draw(Engine* e);

    static          OptionsMenu* Instance() {
        return &m_OptionsMenu;
    }
    void            SelectUp();
    void            SelectDown();
    void            Confirm(Engine* e);

protected:
    OptionsMenu() { }

private:
    static          OptionsMenu m_OptionsMenu;

    int             m_choice;
    Boxes           m_Boxes;
};

#endif
