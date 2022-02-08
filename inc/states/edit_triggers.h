#ifndef EditTriggers_H
#define EditTriggers_H

#include <iostream>
#include <sstream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "object.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

#include <states/input_string.h>

class EditTriggers : public GameState {
public:
    void Init(Engine* eng);
    void Cleanup(Engine* eng);
    void Pause(Engine* eng) {}
    void Resume(Engine* eng);
    void HandleEvents(Engine* eng);
    void Update(Engine* eng);
    void Draw(Engine* eng);

    static EditTriggers* Instance() {
        return &m_EditTriggers;
    }

protected:

    EditTriggers() {}

private:
    enum                {CORNER_NONE, CORNER_NW, CORNER_NE, CORNER_SW, CORNER_SE};

    void                UpdateCamera();
    void                UpdateHud();
    void                DrawTriggers();

    void                LeftMouseButtonPressed();
    void                RightMouseButtonPressed();
    void                MouseMoved();
    void                LeftMouseButtonReleased();
    void                RightMouseButtonReleased();

    Boxes               m_Boxes;
    sf::View            m_View;
    static EditTriggers m_EditTriggers;
    Engine*        e;

    sf::Vector2f        m_viewPos;
    sf::Vector2i        m_mousePos;
    sf::Vector2f        m_worldPos;
    sf::Time            m_delta;

    unsigned            m_currentCorner     = CORNER_NONE;
    Trigger*            m_pCurrentTrigger   = nullptr;
    bool                m_dragging          = false;
    std::vector<sf::RectangleShape>
    m_Rectangles;
    sf::Vector2f        m_prevPos;
};

#endif
