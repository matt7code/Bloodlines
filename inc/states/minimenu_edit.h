#ifndef MINI_MENU_EDIT_STATE_H
#define MINI_MENU_EDIT_STATE_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"

#include "states/edit_map.h"
#include "states/edit_enemy.h"
#include "states/edit_item.h"
#include "states/edit_npc_waypoint.h"
#include "states/edit_anim.h"

class MiniMenu_Edit : public GameState {
public:
    void            Init(Engine* e);
    void            HandleEvents(Engine* e);
    void            Update(Engine* e);
    void            Draw(Engine* e);
    void            Cleanup(Engine* e) {};
    void            Pause(Engine* e) {};
    void            Resume(Engine* e) {};

    static          MiniMenu_Edit* Instance() {
        return &m_MiniMenu_Edit;
    }
    void            SelectUp();
    void            SelectDown();
    void            Confirm(Engine* e);
    void            Update_Hud(Engine* e);

protected:
    MiniMenu_Edit() { }

private:
    static          MiniMenu_Edit m_MiniMenu_Edit;
    bool            m_sync = true;
    int             m_choice;
    Boxes           m_Boxes;

    std::stringstream               HudString;
    sf::Clock                       ClockFramerate;
    float                           prevFpsClock;
    int                             currFrameRate;
    std::vector<int>                frameRates;
};

#endif
