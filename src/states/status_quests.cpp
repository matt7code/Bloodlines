#include "states/status_quests.h"

Status_Quests Status_Quests::m_Status_Quests;

void Status_Quests::Init(Engine* eng) {
    e = eng;
//    m_quests = e->getParty().getQuests();
    m_Boxes.clear();
    m_selection = 0;
    m_offset = 0;

    m_Boxes.addBox("Status", sf::Vector2f(1200, 100), sf::Vector2f(250, 50));
    m_Boxes.addBox("Body", sf::Vector2f(1520, 700), sf::Vector2f(200, 150));

    m_Boxes.getBox("Status").addText("Quests", sf::Vector2f(30, -5), 60);
    m_Boxes.getBox("Status").addText("Press [SPACE] to toggle showing of completed quests.", sf::Vector2f(300, 25), 32);

//    ProcessQuests();
    UpdateBody();

    e->playMusic(Engine::SONG_STATUS);
}

void Status_Quests::HandleEvents(Engine* eng) {
    // Process events
    sf::Event event;
    while(e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
        if(event.type == sf::Event::Closed) {
            e->Quit();
        }
        if(event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
            case sf::Keyboard::Return:
                m_Boxes.PlayCancel();
                e->PopState();
                break;

            case sf::Keyboard::Space:
                m_showCompleted = !m_showCompleted;
                m_selection = m_offset = 0;
                UpdateBody();
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                SelectUp();
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                SelectDown();
                break;

            default:
                break;
            }
        }
    }
}

void Status_Quests::Update(Engine* eng) {
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    ScaleArrow();
}

void Status_Quests::Draw(Engine* eng) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);

    m_Boxes.drawArrow(sf::Vector2f(245, 278 + (m_selection * 53)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);

    if(m_offset > 0)
        m_Boxes.drawArrow(sf::Vector2f(300, 200), 1, m_arrowScale, e->m_RenderWindow, true);
    if((int)m_quests.size() > m_offset + 10)
        m_Boxes.drawArrow(sf::Vector2f(300, 800), 3, m_arrowScale, e->m_RenderWindow, true);
}

void Status_Quests::SelectUp() {
    if(m_selection > 0) --m_selection;
    else if(m_offset > 0) --m_offset;
    std::cout << m_selection << " + " << m_offset << " < " << m_quests.size() << std::endl;
    UpdateBody();
}

void Status_Quests::SelectDown() {
    // The cast to (int) is actually important for the comparisons.
    if(m_selection < 9 && m_selection < (int)m_quests.size() - 1) {
        m_Boxes.PlaySelect();
        ++m_selection;
    } else if(m_selection == 9 && m_selection + m_offset < (int)m_quests.size()) {
        if(m_offset < (int)m_quests.size() - 10) {
            m_Boxes.PlaySelect();
            ++m_offset;
        }
    }
    std::cout << m_selection << " + " << m_offset << " < " << m_quests.size() << std::endl;
    UpdateBody();
}

void Status_Quests::ScaleArrow() {
    int arrow_timer = e->getTimeRunning().getElapsedTime().asMilliseconds() % 2000;
    if(arrow_timer > 1000) {
        arrow_timer -= 1000;
        int down_timer = 1000;
        down_timer -= arrow_timer;
        arrow_timer = down_timer;
    }
    float t = 0.001f * arrow_timer / 2;
    m_arrowScale = sf::Vector2f(t + 1.0f, t + 1.0f);
}

void Status_Quests::UpdateBody() {
    m_quests.clear();
    for (auto q : e->getParty().getQuests()) {
        if(m_showCompleted || q.m_display) m_quests.push_back(q);
    }

    m_Boxes.getBox("Body").clear();

    for(int i = m_offset; i < m_offset + 10; ++i) {
        if(i < (int)m_quests.size()) {
            m_Boxes.getBox("Body").addText(m_quests[i].m_name, sf::Vector2f(60, 80 + ((i - m_offset) * 53)), 36, Box::FONT_NORMAL, sf::Color::Yellow);
        }
    }
    std::cout << "m_quests.size(): " << m_quests.size() << "/" << m_selection + m_offset << std::endl;
    /// Problem
    if(!m_quests.empty()) {
        m_Boxes.getBox("Body").addText(Tool::FormatLine(m_quests[m_selection + m_offset].m_desc, 50), sf::Vector2f(500, 80), 36, Box::FONT_NORMAL, sf::Color::Green);
    } else {
        m_Boxes.getBox("Body").addText("You have no active quests", sf::Vector2f(300, 80), 36, Box::FONT_NORMAL, sf::Color::Green);
    }
}
