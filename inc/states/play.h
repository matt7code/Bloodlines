#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <sstream>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "constants.h"
#include "gamestate.h"
#include "map_level.h"
#include "gameengine.h"
#include "states/edit_map.h"
#include "states/edit_enemy.h"
#include "states/combat.h"
#include "states/minimenu.h"
#include "states/minimenu_edit.h"
#include "states/dialog.h"
#include "boxes.h"
#include "tool.h"
#include "trigger.h"

class Play : public GameState {
public:
    void        Init(Engine* eng);
    void        Cleanup(Engine* e) {}
    void        Pause(Engine* e) {}
    void        Resume(Engine* e);
    void        HandleEvents(Engine* e);
    void        Update(Engine* e);
    void        Draw(Engine* e);
    void        Update_Hud(Engine* e);

    static      Play* Instance() {
        return &m_PlayState;
    }

protected:
    Play() {}

private:
    enum {ENTRY, STEP, EXIT};
    Engine*         e;
    static Play     m_PlayState;

    void            DoSearch(Engine* e);
    void            DoTalkAndSearch(Engine* e);
    void            Move(sf::Vector2f mv);
    bool            ProcessTriggers();
    void            DoClosedDoor(sf::Vector2f& targetLocation, int openDoorTile);
    bool            partyCanPassDoor(sf::Vector2f target);

    Boxes           m_Boxes;
    sf::Time        TimeOfLastCombat;
    sf::Time        m_prevTime;
    long            m_PartyCurrentSteps;
    bool            m_inTrigger;
    void            RunTriggerScript(Trigger& trigger, int part);

    void            LoadShader();
    sf::Shader      m_shader;
};

#endif
