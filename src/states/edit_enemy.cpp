#include "states/edit_enemy.h"

// Enemy & Encounters

EditEnemy EditEnemy::m_EditEnemy;

void EditEnemy::Init(Engine* e) {
    m_Boxes.getBoxes().clear();

    // View
    sf::View tempView;
    tempView.reset(sf::FloatRect(0, 0, 1920, 1080));
    e->m_RenderWindow.setView(tempView);

    m_Boxes.addBox("Desc", sf::Vector2f(500, 500), sf::Vector2f(300,300));
    m_Boxes.getBox("Desc").addText("Enemy Editor", sf::Vector2f(10,0), 48);
}

void EditEnemy::Cleanup(Engine* e) {
}

void EditEnemy::Pause(Engine* e) {
    m_View = e->m_RenderWindow.getView();

}

void EditEnemy::Resume(Engine* e) {
    e->m_RenderWindow.setView(m_View);
}

void EditEnemy::HandleEvents(Engine* e) {
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
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

void EditEnemy::Update(Engine* e) {
    // Center the View
    e->m_View.setCenter(e->getParty().getPos());

    // Update the Level
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
}

void EditEnemy::Draw(Engine* e) {
    // Set the View
    e->m_RenderWindow.setView(e->m_View);

    // Draw the Level
    e->m_Level.Draw(e->m_RenderWindow);

    // Draw the Party
    e->getParty().Draw(e->m_RenderWindow);

    // Reset the View
    e->m_RenderWindow.setView(e->m_ViewBase);

    // Draw the HUD
    m_Boxes.DrawBoxes(e->m_RenderWindow);

        int  j = 0;
    for(unsigned i = 9; i < 10; --i) {
        sf::Sprite temp = e->m_numbers[i];
        temp.setPosition(520 + (32*j++), 520);
        e->m_RenderWindow.draw(temp);

    }
    // Draw Arrow
    //m_Boxes.drawArrow(sf::Vector2f(140.0f, 157.0f + (50.0f * m_choice)), 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow);
}
