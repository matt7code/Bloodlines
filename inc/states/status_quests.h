#ifndef Status_Quests_STATE_H
#define Status_Quests_STATE_H

#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "gameengine.h"
#include "gamestate.h"
#include "quest.h"
#include "boxes.h"
#include "tool.h"


class Status_Quests : public GameState {
public:
    void                        Init(Engine* eng);
    void                        Cleanup(Engine* eng) {}
    void                        Pause(Engine* eng) {}
    void                        Resume(Engine* eng) {}
    void                        HandleEvents(Engine* eng);
    void                        Update(Engine* eng);
    void                        Draw(Engine* eng);
    static                      Status_Quests* Instance() { return &m_Status_Quests; }

protected:
    Status_Quests() { }

private:
    static Status_Quests        m_Status_Quests;
    Engine*                     e;

    Boxes                       m_Boxes;

    void                        SelectRight();
    void                        SelectLeft();
    void                        SelectUp();
    void                        SelectDown();
    void                        UpdateBody();
    void                        ScaleArrow();
    //void                        ProcessQuests();

    sf::Vector2f                m_arrowScale;
    int                         m_selection;
    int                         m_offset;
    std::string                 m_desc;
    bool                        m_showCompleted;
    std::vector<Quest>         m_quests;
    // local reference to quests

};

#endif // Status_Quests_STATE_H
