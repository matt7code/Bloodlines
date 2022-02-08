/*
    Reminders:
    Change the header guard.
    Are you Pushing a State, or Changing a state?
*/

#ifndef EditObject_H
#define EditObject_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

class EditObject : public CGameState
{
public:
    void Init(CGameEngine* eng);
    void Cleanup(CGameEngine* eng) {};
    void Pause(CGameEngine* eng) {};
    void Resume(CGameEngine* eng) {};
    void HandleEvents(CGameEngine* eng);
    void Update(CGameEngine* eng);
    void Draw(CGameEngine* eng);

    static EditObject* Instance() { return &m_EditObject; }

protected:

    EditObject() { }

private:
    Boxes               m_Boxes;
    sf::View            m_View;
    static EditObject   m_EditObject;
    CGameEngine*        e;
};

#endif

