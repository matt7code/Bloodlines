#ifndef MINI_MENU_STATE_H
#define MINI_MENU_STATE_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "gamestate.h"
#include "boxes.h"
#include "gameengine.h"
#include "gamestate.h"

#include "states/tactics.h"
#include "states/menu_options.h"
#include "states/status_stats.h"
#include "states/status_inventory.h"
#include "states/status_quests.h"
#include "states/status_specials.h"

class MiniMenu : public GameState {
public:
    void            Init(Engine* e);
    void            HandleEvents(Engine* e);
    void            Update(Engine* e);
    void            Draw(Engine* e);
    void            Cleanup(Engine* e) {}
    void            Pause(Engine* e) {}
    void            Resume(Engine* e) {}

    static          MiniMenu* Instance() { return &m_MiniMenu; }
    void            SelectUp();
    void            SelectDown();
    void            Confirm(Engine* e);

protected:
    MiniMenu() { }

private:
    static          MiniMenu m_MiniMenu;

    int             m_choice;
    Boxes           m_Boxes;
};

#endif
