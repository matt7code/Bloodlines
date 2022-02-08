#include <SFML/Graphics.hpp>

#include <iostream>
#include "gameengine.h"
#include "gamestate.h"
#include "states/menu_options.h"
#include "states/save_load.h"

OptionsMenu OptionsMenu::m_OptionsMenu;

void OptionsMenu::Init(Engine* e) {
    // View
    sf::View tempView;
    tempView.reset(sf::FloatRect(0, 0, 1920, 1080));
    e->m_RenderWindow.setView(tempView);

    m_Boxes.Menu({"Save", "Load", "Toggle Music", "Toggle Sound", "Quit"}, sf::Vector2f(100.0f, 100.0f));
}

void OptionsMenu::Cleanup(Engine* e) {
    m_Boxes.removeBox("Menu");
}

void OptionsMenu::Pause(Engine* e) {
}

void OptionsMenu::Resume(Engine* e) {
}

void OptionsMenu::HandleEvents(Engine* e) {
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
                m_Boxes.PlayCancel();
                e->PopState();
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                m_Boxes.PlaySelect();
                SelectUp();
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                m_Boxes.PlaySelect();
                SelectDown();
                break;

            case sf::Keyboard::Return:
                m_Boxes.PlayConfirm();
                Confirm(e);
                break;

            default:
                break;
            }
        }
    }
}

void OptionsMenu::Update(Engine* e) {
    // Center the View
    e->m_View.setCenter(e->getParty().getPos());

    // Update the Level
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
}

void OptionsMenu::Draw(Engine* e) {
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

    // Draw Arrow
    m_Boxes.drawArrow(sf::Vector2f(140.0f, 157.0f + (50.0f * m_choice)), 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow);
}

void OptionsMenu::Confirm(Engine* e) {
    //std::cout << "Confirmed Choice: " << m_choice << std::endl;
    switch(m_choice) {
    case 0:
        e->setSaveLoadMode(true);
        e->m_bCameFromMainMenu = false;
        e->PushState( SaveLoad::Instance() );
        break;
    case 1:
        e->setSaveLoadMode(false);
        e->m_bCameFromMainMenu = false;
        e->PushState( SaveLoad::Instance() );
        break;
    case 2:
        e->ToggleMusic();
        break;
    case 3:
        e->ToggleSound();
        break;
    case 4:
        e->Quit();
        break;
    }
}

void OptionsMenu::SelectUp() {
    m_choice--;
    if (m_choice < 0) m_choice = m_Boxes.getNumberOfMenuChoices() - 1;
}

void OptionsMenu::SelectDown() {
    m_choice++;
    if (m_choice > m_Boxes.getNumberOfMenuChoices() - 1) m_choice = 0;
}
