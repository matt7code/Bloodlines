#ifndef STATUS_STATE_H
#define STATUS_STATE_H

#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

const int AVATAR_ROW = -1;

class StateStats : public GameState {
public:
    void                        Init(Engine* e);
    void                        Cleanup(Engine* e);
    void                        Pause(Engine* e);
    void                        Resume(Engine* e);
    void                        HandleEvents(Engine* e);
    void                        Update(Engine* e);
    void                        Draw(Engine* e);
    static                      StateStats* Instance() { return &m_StateStats; }

protected:
    StateStats() { }

private:
    static StateStats           m_StateStats;
    sf::Sprite                  m_Sprite;
    int                         m_CurrentSelectedPartyMember = 0;
    Boxes                       m_Boxes;

    void                        SelectRight(int currentPartyMember, int partySize, Engine* e);
    void                        SelectLeft(int currentPartyMember, int partySize, Engine* e);
    void                        SelectUp(int currentPartyMember, int partySize);
    void                        SelectDown(int currentPartyMember, int partySize);

    void                        AnimateWalk(float seconds);
    void                        FillInBody(Hero& hero, sf::RenderWindow& window);

    int                         m_animFrame = 0;
    int                         m_animFrameSet[4] = {0, 1, 2, 1};
    float                       m_AnimSeconds = 0.0f;
    sf::Time                    m_AnimTime;
    int                         m_statusSlot = AVATAR_ROW;
    std::string                 m_desc;
};

#endif
