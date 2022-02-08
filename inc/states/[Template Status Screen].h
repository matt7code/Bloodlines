#ifndef Template_STATE_H
#define Template_STATE_H

#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

const int AVATAR_ROW = -1;

class Template : public CGameState
{
public:
    void                        Init(CGameEngine* e);
    void                        Cleanup(CGameEngine* e);
    void                        Pause(CGameEngine* e);
    void                        Resume(CGameEngine* e);
    void                        HandleEvents(CGameEngine* e);
    void                        Update(CGameEngine* e);
    void                        Draw(CGameEngine* e);
    static                      Template* Instance() { return &m_Template; }

    void                        SelectRight(int currentPartyMember, int partySize, CGameEngine* e);
    void                        SelectLeft(int currentPartyMember, int partySize, CGameEngine* e);
    void                        SelectUp(int currentPartyMember, int partySize);
    void                        SelectDown(int currentPartyMember, int partySize);

    void                        AnimateWalk(float seconds);
    void                        FillInBody(Hero& hero, sf::RenderWindow& window);

protected:                      Template() { }

private:
    static Template             m_Template;
    sf::Sprite                  m_Sprite;
    int                         m_CurrentSelectedPartyMember = 0;
    Boxes                       m_Boxes;
    int                         m_animFrame = 0;
    int                         m_animFrameSet[4] = {0, 1, 2, 1};
    float                       m_AnimSeconds = 0.0f;
    sf::Time                    m_AnimTime;
    int                         m_statusSlot = AVATAR_ROW;
    std::string                 m_desc;
};

#endif
