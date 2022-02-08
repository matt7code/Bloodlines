#ifndef Template_H
#define Template_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

/*
    Reminders:
    Change the header guard.
    Are you Pushing a State, or Changing a state?
*/

class Template : public CGameState
{
public:
    void    Init(CGameEngine* engine);
    void    HandleEvents(CGameEngine* engine);
    void    Update(CGameEngine* engine);
    void    Draw(CGameEngine* engine);
    void    Cleanup(CGameEngine* engine) {};
    void    Pause(CGameEngine* engine) {};
    void    Resume(CGameEngine* engine) {};
    static  Template* Instance() { return &m_Template; }

protected:
    Template() {}

private:
    static Template    m_Template;
    CGameEngine*        e;
    Boxes               m_Boxes;
};

#endif
