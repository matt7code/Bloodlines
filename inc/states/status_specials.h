#ifndef Status_Specials_STATE_H
#define Status_Specials_STATE_H

#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "special.h"
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

class Status_Specials : public GameState {
public:
    void                        Init(Engine* eng);
    void                        Cleanup(Engine* eng) {}
    void                        Pause(Engine* eng) {}
    void                        Resume(Engine* eng) {}
    void                        HandleEvents(Engine* eng);
    void                        Update(Engine* eng);
    void                        Draw(Engine* eng);
    static                      Status_Specials* Instance() { return &m_Status_Specials; }

protected:
    Status_Specials() { }

private:
    static Status_Specials      m_Status_Specials;
    Engine*                     e;

    const int                   AVATAR_ROW = -1;
    sf::Sprite                  m_Sprite;
    int                         m_CurrentSelectedPartyMember = 0;
    Boxes                       m_Boxes;

    void                        SelectRight();
    void                        SelectLeft();
    void                        SelectUp();
    void                        SelectDown();
    void                        AnimateWalk(float seconds);
    void                        UpdateBody(Hero& hero, sf::RenderWindow& window);
    void                        SetStatus(unsigned hero_class);
    void                        ScaleArrow();

    int                         m_animFrame = 0;
    int                         m_animFrameSet[4] = {0, 1, 2, 1};
    float                       m_AnimSeconds = 0.0f;
    sf::Time                    m_AnimTime;
    sf::Vector2f                m_arrowScale;

    int                         m_selection = AVATAR_ROW;
    int                         m_offset;

    std::string                 m_desc;
    std::vector<std::string>    m_specialList;
    //std::vector<std::string>    m_spellDescs;
};

#endif // Status_Specials_STATE_H
