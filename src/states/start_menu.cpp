#include "states/start_menu.h"

CStartState CStartState::m_StartState;

void CStartState::Init(Engine* e) {
    // Load a sprite to display
    if (!m_texture.loadFromFile("graphics/start_menu.png"))
        std::cout << "Error loading graphics/start_menu.png" << std::endl;
    m_startScreen.setTexture(m_texture);

    // Load a menu sprite sheet
    if (!m_startTexture.loadFromFile("graphics/game_menu_sprites.png"))
        std::cout << "Error loading game_menu_sprites.png" << std::endl;

    m_currentSelection = 0;

    // Load sword!
    m_Sword.setTexture(m_startTexture);
    m_Sword.setScale(0.10f, 0.10f);
    m_Sword.setOrigin(sf::Vector2f(m_Sword.getLocalBounds().width / 2, m_Sword.getLocalBounds().height / 2));
    m_Sword.setRotation(180.0f);
    m_Sword.setPosition(300, 350 + (60 * m_currentSelection));

    m_Sword2.setTexture(m_startTexture);
    m_Sword2.setScale(0.10f, 0.10f);
    m_Sword2.setOrigin(sf::Vector2f(m_Sword.getLocalBounds().width / 2, m_Sword.getLocalBounds().height / 2));
    //m_Sword2.setRotation(0.0f);
    m_Sword2.setPosition(500, 350 + (60 * m_currentSelection));

    // View
    sf::View view_game_menu;
    view_game_menu.reset(sf::FloatRect(0, 0, 800, 600));
    e->m_RenderWindow.setView(view_game_menu);

    m_numberOfTicks = 1;
}

void CStartState::Cleanup(Engine* e) {
    e->stopMusic();
}

void CStartState::Pause(Engine* e) {
    m_savedView = e->m_RenderWindow.getView();
}

void CStartState::Resume(Engine* e) {
    e->m_RenderWindow.setView(m_savedView);
}

void CStartState::HandleEvents(Engine* e) {
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
        if (event.type == sf::Event::Closed) {
            e->Cleanup();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                UpChoice();
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                DownChoice();
                break;

            case sf::Keyboard::Return:
                Select(e);
                break;

            default:
                break;
            }
        }
    }
}

void CStartState::Update(Engine* e) {
    m_numberOfTicks++;
    m_offset = 10 * std::sin(m_numberOfTicks * 0.025 * PI);
    m_Sword.setPosition(sf::Vector2f(275 - m_offset, 350 + (60 * m_currentSelection)));
    m_Sword2.setPosition(sf::Vector2f(525 + m_offset, 350 + (60 * m_currentSelection)));

}

void CStartState::Draw(Engine* e) {
    e->m_RenderWindow.draw(m_startScreen);
    e->m_RenderWindow.draw(m_Sword);
    e->m_RenderWindow.draw(m_Sword2);
}

void CStartState::UpChoice() {
    m_Boxes.PlaySelect();
    if (m_currentSelection == 0) m_currentSelection = 2;
    else --m_currentSelection;
}

void CStartState::DownChoice() {
    m_Boxes.PlaySelect();
    if (m_currentSelection == 2) m_currentSelection = 0;
    else ++m_currentSelection;
}

void CStartState::Select(Engine* e) {
    m_Boxes.PlayConfirm();
    if (m_currentSelection == 0) {
        e->m_bCameFromMainMenu = true;
        e->setSaveLoadMode(false);
        e->PushState( SaveLoad::Instance() );
    }
    if (m_currentSelection == 1) e->PushState( CharacterDesigner::Instance() );
    if (m_currentSelection == 2) e->Quit();
}
