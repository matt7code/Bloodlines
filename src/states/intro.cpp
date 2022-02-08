#include <SFML/Graphics.hpp>
#include <iostream>
#include "gameengine.h"
#include "gamestate.h"
#include "states/intro.h"
#include "states/start_menu.h"

CIntroState CIntroState::m_IntroState;

void CIntroState::Init(Engine* e) {
    // Load a sprite to display
    if (!m_introTexture.loadFromFile("graphics/intro.png"))
        std::cout << "Error loading intro.png" << std::endl;

    m_introScreen.setTexture(m_introTexture);

    // View
    e->m_View.reset(sf::FloatRect(0, 0, 1920, 1080));
    e->m_RenderWindow.setView(e->m_View);

#ifdef DEBUG
    e->ToggleMusic();
#endif

    e->playMusic(Engine::SONG_THEME);
}

void CIntroState::HandleEvents(Engine* e) {
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
        if (event.type == sf::Event::Closed)        e->Quit();
        if (event.type == sf::Event::KeyPressed)    e->ChangeState( CStartState::Instance() );
    }
}

void CIntroState::Draw(Engine* e) {
    e->m_RenderWindow.draw(m_introScreen);
}
