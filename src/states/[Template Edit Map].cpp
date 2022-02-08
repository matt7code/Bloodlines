#include "states/edit_objects.h"

EditObject EditObject::m_EditObject;

void EditObject::Init(CGameEngine* eng)
{
    e = eng;
    m_Boxes.getBoxes().clear();
    m_Boxes.addBox("Desc", sf::Vector2f(50, 50), sf::Vector2f(300,64)).addText("EditObject", sf::Vector2f(10,0), 48);
}

void EditObject::HandleEvents(CGameEngine* eng)
{
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event))
    {
        // Close window : exit
        if (event.type == sf::Event::Closed)
        {
            e->Quit();
        }
        if (event.type == sf::Event::KeyPressed)
        {
            switch(event.key.code)
            {
            case sf::Keyboard::Escape:
                e->PopState();
                break;

            default:
                break;
            }
        }
    }
}

void EditObject::Update(CGameEngine* eng)
{
    e->m_View.setCenter(e->getParty().getPos());
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
}

void EditObject::Draw(CGameEngine* eng)
{
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->getParty().Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    //m_Boxes.drawArrow(sf::Vector2f(140.0f, 157.0f + (50.0f * m_choice)), 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow);
}

