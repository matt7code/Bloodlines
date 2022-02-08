#include "states/status_specials.h"

Status_Specials Status_Specials::m_Status_Specials;

void Status_Specials::Init(Engine* eng) {
    e = eng;
    m_Boxes.clear();
    m_selection = Status_Specials::AVATAR_ROW;
    m_CurrentSelectedPartyMember = 0;

    m_Boxes.addBox("Body", sf::Vector2f(1520, 700), sf::Vector2f(200, 150));
    m_Boxes.addBox("Status", sf::Vector2f(200, 100), sf::Vector2f(250, 50));
    UpdateBody(e->getParty().getMember(m_CurrentSelectedPartyMember), e->m_RenderWindow);

    // Characters:
    for(int i = 0; i < e->getParty().getSize(); ++i) {
        std::stringstream ss;
        ss << i;
        m_Boxes.addBox("PartyMember" + ss.str(), sf::Vector2f(100, 100), sf::Vector2f(600 + (150 * i), 80));
    }
    m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
    e->playMusic(Engine::SONG_STATUS);
}

void Status_Specials::HandleEvents(Engine* eng) {
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

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                SelectRight();
                break;

            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                SelectLeft();
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

void Status_Specials::Update(Engine* eng) {
    // Update the Level
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    AnimateWalk(e->getTimeRunning().getElapsedTime().asSeconds() - m_AnimTime.asSeconds());
    ScaleArrow();
}

void Status_Specials::Draw(Engine* eng) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);

    // Draw the Party in the slots
    sf::Sprite slotAvatar;
    for(int i = 0; i < e->getParty().getSize(); ++i) {
        slotAvatar = e->getParty().getMember(i).getSprite();

        sf::Vector2i textureSheetBase = e->getParty().getMember(i).getTextureSheetBase();

        if(i == m_CurrentSelectedPartyMember) {
            /*if (m_selection == AVATAR_ROW)*/
            m_Boxes.drawArrow(sf::Vector2f(600 + (150 * i), 70), 3, sf::Vector2f(1.0f, 3.0f), e->m_RenderWindow);
            slotAvatar.setTextureRect(sf::IntRect(textureSheetBase.x + (m_animFrameSet[m_animFrame] * TILE_SIZE_X), textureSheetBase.y, 32, 32));
        } else {
            slotAvatar.setTextureRect(sf::IntRect(textureSheetBase.x + 32, textureSheetBase.y, 32, 32));
        }
        slotAvatar.setOrigin(0, 0);
        slotAvatar.setPosition(sf::Vector2f(595 + (150 * i), 73));
        slotAvatar.setScale(3.5f, 3.5f);
        e->m_RenderWindow.draw(slotAvatar);
    }
    if(m_selection != AVATAR_ROW) {
        int y = m_selection;
        //int x = m_currSelection / 10;
        //m_Boxes.drawArrow(sf::Vector2f(245 + (x * 500), 278 + (y * 53)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
        m_Boxes.drawArrow(sf::Vector2f(245, 278 + (y * 53)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
        //m_Boxes.drawArrow(sf::Vector2f(245 + (x * 500), 278), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
    }
    if(m_offset > 0)
        m_Boxes.drawArrow(sf::Vector2f(300, 200), 1, m_arrowScale, e->m_RenderWindow, true);
    if((int)m_specialList.size() > m_offset + 10)
        m_Boxes.drawArrow(sf::Vector2f(300, 800), 3, m_arrowScale, e->m_RenderWindow, true);
}

void Status_Specials::SelectUp() {
    if(m_selection != AVATAR_ROW) m_Boxes.PlaySelect();
    if(m_selection > 0) --m_selection;
    else if (m_offset > 0) --m_offset;
    else {
        m_selection = AVATAR_ROW;
    }
    std::cout << m_selection << " + " << m_offset << " < " << m_specialList.size() << std::endl;
    UpdateBody(e->getParty().getMember(m_CurrentSelectedPartyMember), e->m_RenderWindow);
}

void Status_Specials::SelectDown() {
    // The cast to (int) is actually important for the comparisons.
    if(m_selection < 9 && m_selection < (int)m_specialList.size() - 1) {
        m_Boxes.PlaySelect();
        ++m_selection;
    } else if(m_selection == 9 && m_selection + m_offset < (int)m_specialList.size()) {
        if (m_offset < (int)m_specialList.size() - 10) {
            m_Boxes.PlaySelect();
            ++m_offset;
        }
    }
    std::cout << m_selection << " + " << m_offset << " < " << m_specialList.size() << std::endl;
    UpdateBody(e->getParty().getMember(m_CurrentSelectedPartyMember), e->m_RenderWindow);
}

void Status_Specials::AnimateWalk(float seconds) {
    m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
    m_AnimSeconds += seconds;
    if(m_AnimSeconds > 0.25f) {
        m_AnimSeconds = 0.0f;
        m_animFrame++;
        if(m_animFrame == 4) m_animFrame = 0;
    }
}

void Status_Specials::SelectLeft() {
    m_selection = AVATAR_ROW;
    m_offset = 0;
    m_Boxes.PlaySelect();
    --m_CurrentSelectedPartyMember;
    if(m_CurrentSelectedPartyMember < 0) m_CurrentSelectedPartyMember = e->getParty().getSize() - 1;
    UpdateBody(e->getParty().getMember(m_CurrentSelectedPartyMember), e->m_RenderWindow);
}

void Status_Specials::SelectRight() {
    m_selection = AVATAR_ROW;
    m_offset = 0;
    m_Boxes.PlaySelect();
    ++m_CurrentSelectedPartyMember;
    if(m_CurrentSelectedPartyMember > e->getParty().getSize() - 1) m_CurrentSelectedPartyMember = 0;
    UpdateBody(e->getParty().getMember(m_CurrentSelectedPartyMember), e->m_RenderWindow);
}

void Status_Specials::SetStatus(unsigned hero_class) {
    m_Boxes.getBox("Status").clear();
    std::string special;
    switch(hero_class) {
    case CLASS_WARRIOR:
        special = "Shouts";
        break;
    case CLASS_CLERIC:
        special = "Prayer";
        break;
    case CLASS_WIZARD:
        special = "Spells";
        break;
    case CLASS_ROGUE:
        special = "Tricks";
        break;
    default:
        special = "Error";
        break;
    }
    m_Boxes.getBox("Status").addText(special, sf::Vector2f(10, 5), 60);
}

void Status_Specials::UpdateBody(Hero& hero, sf::RenderWindow& window) {
    SetStatus(hero.getClass());
    m_Boxes.getBox("Body").clear();
    m_specialList.clear();

    for(auto s : hero.getSpells()) {
        m_specialList.push_back(Special(s).m_name);
    }

    for(int i = m_offset; i < m_offset + 10; ++i) {
        if(i < (int)m_specialList.size()) {
            //std::cout << "i: " << i << " Offset: " << m_offset << std::endl;
            m_Boxes.getBox("Body").addText(m_specialList[i], sf::Vector2f(60, 80 + ((i - m_offset) * 53)), 48);
        }
    }
    if(m_selection != AVATAR_ROW) {
        std::string desc = Tool::FormatLine(Special(hero.getSpells()[m_selection + m_offset]).m_desc, 50);
        std::stringstream ss;

        ss << desc << "\n\nMagic Cost: " << Special(hero.getSpells()[m_selection + m_offset]).m_cost;

        m_Boxes.getBox("Body").addText(ss.str(), sf::Vector2f(500, 80), 36, Box::FONT_NORMAL, sf::Color::Yellow);
    }
}

void Status_Specials::ScaleArrow()
{
    int arrow_timer = e->getTimeRunning().getElapsedTime().asMilliseconds() % 2000;
    if (arrow_timer > 1000) {
        arrow_timer -= 1000;
        int down_timer = 1000;
        down_timer -= arrow_timer;
        arrow_timer = down_timer;
    }
    float t = 0.001f * arrow_timer / 2;
    m_arrowScale = sf::Vector2f(t + 1.0f, t + 1.0f);
}
