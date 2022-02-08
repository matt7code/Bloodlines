#include "states/Template.h"

/// Reminder: Change the header guard!
/// Reminder: Are you Pushing a State, or Changing a state?

Template Template::m_Template;

void Template::Init(CGameEngine* engine) {
    e = engine;
    m_Boxes.getBoxes().clear();
    m_Boxes.addBox("Template");
    m_Boxes.getBox("Template").addText(Tool::FormatLine("Welcome to your new Template State!", 21));
}

void Template::HandleEvents(CGameEngine* engine) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            e->Quit();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                e->PopState();
                break;

            default:
                break;
            }
        }
    }
}

void Template::Update(CGameEngine* engine) {
    e->m_View.setCenter(e->getParty().getPos());
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
}

void Template::Draw(CGameEngine* engine) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->getParty().Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
}
