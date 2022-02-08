#include "states/tactics.h"

using v2f = sf::Vector2f;

Tactics Tactics::m_TacticsState;

void Tactics::Init(Engine* e) {
    m_Boxes.getBoxes().clear();
    e->m_View.reset(sf::FloatRect(0, 0, 1920, 1080));
    e->m_View.zoom(0.5f);
    e->m_RenderWindow.setView(e->m_View);

    // Center the View
    e->m_View.setCenter(e->getParty().getPos());
    e->playMusic(Engine::SONG_STATUS);

    m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
    heroSelected = false;
    m_submenu_selection = 0;
    m_CurrentSelectionX = 0;
    m_CurrentSelectionY = 0;
    m_section = SECTION_NORMAL;
    m_PartySize = e->getParty().getSize() - 1;

    m_sprite.setTexture(e->getParty().getTexture());

    m_chars.clear();
    for (int i = 0; i != 40; i++) {
        int x = i % 4;
        int y = i / 4;
        m_sprite.setTextureRect(sf::IntRect((x * 96) + 32, y * 128, 32, 32));
        m_sprite.setOrigin(Tool::centerSprite(m_sprite));
        m_sprite.setScale(4.0f, 4.0f);
        m_chars.push_back(m_sprite);
    }
    PositionCharacterRing(0);
    DrawTactics(e);
}

void Tactics::Cleanup(Engine* e) {
    m_Boxes.getBoxes().clear();
}

void Tactics::Pause(Engine* e) {
    e->pauseMusic();
}

void Tactics::Resume(Engine* e) {
    e->playMusic(Engine::SONG_STATUS);
}

void Tactics::HandleEvents(Engine* e) {
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
                if (m_section == SECTION_NORMAL) e->PopState();
                else {
                    if(m_section == SECTION_NAME) {
                        m_Boxes.removeBox("Name");
                        m_Boxes.removeBox("Alphabet");
                        m_Boxes.removeBox("Instructions");
                    } else if(m_section == SECTION_AVATAR) {
                        m_Boxes.removeBox("Avatar");
                    } else if(m_section == SECTION_RECRUIT) {
                        m_Boxes.removeBox("Recruit");
                        m_Boxes.removeBox("RecruitTitle");
                    } else if(m_section == SECTION_REMOVE) {
                        m_Boxes.removeBox("Remove");
                    }
                    m_section = SECTION_NORMAL;
                }
                break;

            case sf::Keyboard::M:
                m_reportMouse = !m_reportMouse;
                if(m_reportMouse) e->m_RenderWindow.setMouseCursorVisible(true);
                else e->m_RenderWindow.setMouseCursorVisible(false);
                break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                SelectRight(e);
                break;

            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                SelectLeft(e);
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                SelectUp(e);
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                SelectDown(e);
                break;

            case sf::Keyboard::Return:
                SelectConfirm(e);
                break;

            default:
                break;
            }
            if(m_section == SECTION_NAME) {
                int x = m_curSymbol % 26;
                int y = m_curSymbol / 26;
                m_arrowPosName = sf::Vector2f(355 + (x * 48), 390 + (y * 95));
                if (m_curSymbol == 91) m_arrowPosName = sf::Vector2f(380 + (x * 48), 385 + (y * 95));
                if (m_curSymbol == 95) m_arrowPosName = sf::Vector2f(380 + (x * 48), 385 + (y * 95));
                if (m_curSymbol == 100) m_arrowPosName = sf::Vector2f(355 + (x * 48), 385 + (y * 95));
            }
        }
    }
}

void Tactics::Update(Engine* e) {
    // Mouse reporting for debug
    if(m_reportMouse) {
        sf::Vector2i currPos = sf::Mouse::getPosition(e->m_RenderWindow);

        float x = (1920.0f / e->m_RenderWindow.getSize().x) * currPos.x;
        float y = (1080.0f / e->m_RenderWindow.getSize().y) * currPos.y;
        currPos = sf::Vector2i(x, y);

        if (m_MousePos != currPos) {
            m_MousePos = currPos;
            std::cout << m_MousePos.x << ", " << m_MousePos.y << std::endl;
        }
    }

    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);

    switch (m_section) {
    case SECTION_NORMAL:
        AnimateWalk(e->getTimeRunning().getElapsedTime().asSeconds() - m_AnimTime.asSeconds());
        m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
        break;

    case SECTION_NAME:
        m_Boxes.getBox("Name").getText().clear();
        if ((e->getTimeRunning().getElapsedTime().asMilliseconds() % 1000) > 500) {
            m_Boxes.getBox("Name").addText(std::string(m_TargetName.size(), ' ') + '_', sf::Vector2f(20, -2), 64);
        }
        m_Boxes.getBox("Name").addText(m_TargetName, sf::Vector2f(20, -2), 64);
        break;

    case SECTION_AVATAR:
        AnimateWalk(e->getTimeRunning().getElapsedTime().asSeconds() - m_AnimTime.asSeconds());
        m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
        break;

    case SECTION_RECRUIT:
        break;
    case SECTION_REMOVE:
        break;
    }
}

void Tactics::Draw(Engine* e) {
    // Set the View
    e->m_RenderWindow.setView(e->m_View);

    // Draw the level.
    e->m_Level.Draw(e->m_RenderWindow);

    // Reset the View
    e->m_RenderWindow.setView(e->m_ViewBase);

    // Draw the HUD
    m_Boxes.DrawBoxes(e->m_RenderWindow);

    // Draw the Party in the slots
    sf::Sprite slotAvatar;
    for (int i = 0; i < e->getParty().getSize(); ++i) {
        slotAvatar = e->getParty().getMember(i).getSprite();
//        m_Avatar = e->getParty().getMember(i).getAvatar();
        sf::Vector2i textureSheetBase = e->getParty().getMember(i).getTextureSheetBase();
        if( i == m_CurrentSelectionX ) {
            if(!heroSelected)
                m_Boxes.drawArrow(sf::Vector2f(650 + (150 * i), 60), 3, sf::Vector2f(1.0f, 3.0f), e->m_RenderWindow, true);
            else
                m_Boxes.drawArrow(sf::Vector2f(650 + (150 * i), 200), 1, sf::Vector2f(1.0f, 3.0f), e->m_RenderWindow, true);
            if(m_section == SECTION_AVATAR) {
                textureSheetBase = sf::Vector2i((m_Avatar % 4) * 96, (m_Avatar / 4) * 128);
            }
            slotAvatar.setTextureRect(sf::IntRect(textureSheetBase.x + (m_animFrameSet[m_animFrame] * TILE_SIZE_X), textureSheetBase.y, 32, 32));
        } else {
            slotAvatar.setTextureRect(sf::IntRect(textureSheetBase.x + 32, textureSheetBase.y, 32, 32));
        }
        slotAvatar.setOrigin(0, 0);
        slotAvatar.setPosition(sf::Vector2f(595 + (150 * i), 73));
        slotAvatar.setScale(3.5f, 3.5f);
        e->m_RenderWindow.draw(slotAvatar);
    }

    switch(m_section) {
    case SECTION_NORMAL:
        m_Boxes.drawArrow(sf::Vector2f(400, 280 + (m_CurrentSelectionY * 100)), 2, sf::Vector2f(2.0, 2.0), e->m_RenderWindow, true);
        break;
    case SECTION_NAME:
        m_Boxes.drawArrow(m_arrowPosName, 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow);
        break;
    case SECTION_AVATAR:
        for (const auto& s : m_chars) e->m_RenderWindow.draw(s);
        m_Boxes.drawArrow(sf::Vector2f(1007, 920), 1, sf::Vector2f(3.0f, 3.0f), e->m_RenderWindow);
        break;
    case SECTION_RECRUIT:
        if(e->getParty().getSize() < 7)
            m_Boxes.drawArrow(sf::Vector2f(1270, 776 + (40 * m_submenu_selection)), 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow, true);
        break;
    case SECTION_REMOVE:
        if(e->getParty().getSize() > 1)
            m_Boxes.drawArrow(sf::Vector2f(922, 668 + (40 * m_submenu_selection)), 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow, true);
        break;
    }
}

void Tactics::AnimateWalk(float seconds) {
    m_AnimSeconds += seconds;
    if (m_AnimSeconds > 0.25f) {
        m_AnimSeconds = 0.0f;
        m_animFrame++;
        if(m_animFrame == 4) m_animFrame = 0;
    }
}

void Tactics::SelectConfirm(Engine* e) {
    bool newGuy = false;
    int i = 0;
    std::stringstream ss;
    std::string upper;

    switch (m_section) {
    case SECTION_NORMAL: // Main Menu
        switch(m_CurrentSelectionY) {
        case SECTION_NORMAL:
            m_Boxes.PlayConfirm();
            heroSelected = !heroSelected;
            break;
        case SECTION_NAME:
            m_Boxes.PlayConfirm();
            m_TargetName = e->getParty().getMember(m_CurrentSelectionX).getName();
            m_arrowPosName = sf::Vector2f(355 + ((m_curSymbol % 26) * 48), 390 + ((m_curSymbol / 26) * 90));
            m_section = SECTION_NAME;
            DrawName();
            break;
        case SECTION_AVATAR:
            m_Boxes.PlayConfirm();
            m_Avatar = e->getParty().getMember(m_CurrentSelectionX).getAvatar();
            PositionCharacterRing(m_Avatar);
            m_section = SECTION_AVATAR;
            DrawAvatar();
            break;
        case SECTION_RECRUIT:
            m_Boxes.PlayConfirm();
            if(e->getParty().getSize() == 7) {
                m_Boxes.addBox("Recruit", v2f(360, 90), v2f(740, 400));
                m_Boxes.getBox("Recruit").addText("Your party is full.", v2f(10, 20), 36);
            } else {
                m_Boxes.PlayConfirm();
                m_Boxes.addBox("Recruit", v2f(340, 200), v2f(1250, 730));
                m_Boxes.addBox("RecruitTitle", v2f(450, 60), v2f(1200, 660));

                std::vector<int> levels;
                for(auto& m : e->getParty().getMembers())
                    levels.push_back(m.getLevel());
                float avg = accumulate(levels.begin(), levels.end(), 0.0) / levels.size();
                average_level = std::round(avg);

                std::stringstream ss;
                ss << "Recruit (Level " << average_level << ")";
                m_Boxes.getBox("RecruitTitle").addText(ss.str(), v2f(10, -5), 48);

                ss.clear();
                ss.str(std::string());
                ss << "Warrior  " << 500 * average_level << "g\nRogue    " << 600 * average_level << "g\nCleric   " << 700 * average_level << "g\nWizard   " << 800 * average_level << "g";
                m_Boxes.getBox("Recruit").addText(ss.str(), v2f(40, 20), 36);
            }
            m_section = SECTION_RECRUIT;
            break;
        case SECTION_REMOVE:
            m_Boxes.PlayConfirm();
            if(e->getParty().getSize() == 1) {
                m_Boxes.addBox("Remove", v2f(580, 90), v2f(740, 400));
                m_Boxes.getBox("Remove").addText("You can't remove your last hero.", v2f(10, 20), 36);
            } else {
                m_Boxes.addBox("Remove", v2f(800, 400), v2f(560, 350));
                m_Boxes.getBox("Remove").addText("Remove", v2f(326, 52), 48, Box::FONT_MESSY, sf::Color::Red);
                for (auto c : e->getParty().getMember(m_CurrentSelectionX).getName()) upper += std::toupper(c);
                m_Boxes.getBox("Remove").addText(upper, v2f((370 - ((upper.size() * 23) / 2)), 105), 60, Box::FONT_NORMAL, sf::Color::Red);
                m_Boxes.getBox("Remove").addText("from the party...", v2f(225, 173), 48, Box::FONT_MESSY, sf::Color::Red);
                m_Boxes.getBox("Remove").addText("Are you sure?\n     Yes\n     No", v2f(295, 250), 36, Box::FONT_NORMAL);
            }
            m_section = SECTION_REMOVE;
            m_submenu_selection = 1;
            break;
        default:
            break;
        }
        break;
    case SECTION_NAME:
        if(m_curSymbol == 91) m_Boxes.PlayCancel(); // Erase makes cancel sound
        else m_Boxes.PlayConfirm();
        DoLetter(m_curSymbol, e);
        break;
    case SECTION_AVATAR:
        m_Boxes.PlayConfirm();
        m_Boxes.removeBox("Avatar");
        e->getParty().getMember(m_CurrentSelectionX).setAvatar(m_Avatar);
        m_section = SECTION_NORMAL;
        break;
    case SECTION_RECRUIT:
        if(e->getParty().getSize() == 7) break;
        m_Boxes.PlayConfirm();
        // Check the cost based on level
        if (m_submenu_selection == 0 && e->getParty().getPartyGold() > (average_level * 500)) { // Warrior
            e->getParty().modPartyGold(average_level * -500);
            e->getParty().AddMember(Tool::getRand(0, 39), average_level, CLASS_WARRIOR, Tool::generateName());
            newGuy = true;
        } else if (m_submenu_selection == 1 && e->getParty().getPartyGold() > (average_level * 600)) { // Rogue
            e->getParty().modPartyGold(average_level * -600);
            e->getParty().AddMember(Tool::getRand(0, 39), average_level, CLASS_ROGUE, Tool::generateName());
            newGuy = true;
        } else if (m_submenu_selection == 2 && e->getParty().getPartyGold() > (average_level * 700)) { // Cleric
            e->getParty().modPartyGold(average_level * -700);
            e->getParty().AddMember(Tool::getRand(0, 39), average_level, CLASS_CLERIC, Tool::generateName());
            newGuy = true;
        } else if (m_submenu_selection == 3 && e->getParty().getPartyGold() > (average_level * 800)) { // Wizard
            e->getParty().modPartyGold(average_level * -800);
            e->getParty().AddMember(Tool::getRand(0, 39), average_level, CLASS_WIZARD, Tool::generateName());
            newGuy = true;
        } else {
            //m_Boxes.PlayBroke();
        }
        if (newGuy) {
            m_Boxes.PlayConfirm();
            // Draw additional box
            i = e->getParty().getSize() - 1;
            m_PartySize++;
            ss << i;
            m_Boxes.addBox("PartyMember" + ss.str(), sf::Vector2f(100, 100), sf::Vector2f(600 + (150 * i), 80));
            m_Boxes.removeBox("Recruit");
            m_Boxes.removeBox("RecruitTitle");
            m_section = SECTION_NORMAL;
        }
        break;
    case SECTION_REMOVE:
        if(e->getParty().getSize() == 1) break;
        m_Boxes.PlayConfirm();
        if(m_submenu_selection == 1) { // cancel remove member
            m_Boxes.removeBox("Remove");
            m_section = SECTION_NORMAL;
        } else { // remove member
            m_PartySize--;
            e->getParty().getMembers().erase(e->getParty().getMembers().begin() + m_CurrentSelectionX);
            ss.clear();
            ss.str(std::string());
            ss << "PartyMember" << e->getParty().getMembers().size();
            m_Boxes.removeBox(ss.str());
            m_Boxes.removeBox("Remove");
            m_CurrentSelectionX = 0;
            m_section = SECTION_NORMAL;
        }
        break;
    }
}

void Tactics::SelectLeft(Engine* e) {
    std::stringstream ss;
    int tempPos = m_CurrentSelectionX;

    switch(m_section) {
    case SECTION_NORMAL:
        m_Boxes.PlaySelect();
        --m_CurrentSelectionX;
        if (m_CurrentSelectionX < 0) m_CurrentSelectionX = m_PartySize;

        LOG(m_CurrentSelectionX);
        m_Boxes.getBox("Selection").getText().clear();
        ss << "The selected Hero is " << e->getParty().getMember(m_CurrentSelectionX).getName() << ", a level " << e->getParty().getMember(m_CurrentSelectionX).getLevel() << " " << e->getParty().getMember(m_CurrentSelectionX).getClassName(e->getParty().getMember(m_CurrentSelectionX).getClass()) << ".";
        m_Boxes.getBox("Selection").addText(ss.str(), v2f(0, 0), 32);

        if(heroSelected) {
            std::iter_swap(e->getParty().getMembers().begin() + tempPos, e->getParty().getMembers().begin() + m_CurrentSelectionX);
            e->getParty().MovePartyTo(e->getParty().getPos());
        }
        break;
    case SECTION_NAME:
        m_Boxes.PlaySelect();
        switch (m_curSymbol) {
        case 78:
            m_curSymbol = 100;
            break;
        case 100:
            m_curSymbol = 95;
            break;
        case 95:
            m_curSymbol = 91;
            break;
        case 0:
            m_curSymbol = 25;
            break;
        default:
            m_curSymbol--;
            break;
        }
        break;
    case SECTION_AVATAR:
        m_Boxes.PlaySelect();
        m_Avatar++;
        if(m_Avatar > 39) m_Avatar = 0;
        m_textureBase.x = m_Avatar % 4;
        m_textureBase.y = m_Avatar / 4;
        m_textureBase.x *= 96;
        m_textureBase.y *= 128;
        PositionCharacterRing(m_Avatar);
        break;
    case SECTION_RECRUIT:
        break;
    case SECTION_REMOVE:
        break;
    }
}

void Tactics::SelectRight(Engine* e) {
    std::stringstream ss;
    int tempPos = m_CurrentSelectionX;
    //Hero& hero = e->getParty().getMember(m_CurrentSelectionX);

    switch(m_section) {
    case SECTION_NORMAL:
        m_Boxes.PlaySelect();
        ++m_CurrentSelectionX;
        if(m_CurrentSelectionX > m_PartySize) m_CurrentSelectionX = 0;

        LOG(m_CurrentSelectionX);
        m_Boxes.getBox("Selection").getText().clear();
        //hero = e->getParty().getMember(m_CurrentSelectionX);
        ss << "The selected Hero is " << e->getParty().getMember(m_CurrentSelectionX).getName() << ", a level " << e->getParty().getMember(m_CurrentSelectionX).getLevel() << " " << e->getParty().getMember(m_CurrentSelectionX).getClassName(e->getParty().getMember(m_CurrentSelectionX).getClass()) << ".";
        m_Boxes.getBox("Selection").addText(ss.str(), v2f(0, 0), 32);

        if(heroSelected) {
            std::iter_swap(e->getParty().getMembers().begin() + tempPos, e->getParty().getMembers().begin() + m_CurrentSelectionX);
            e->getParty().MovePartyTo(e->getParty().getPos());
        }
        break;
    case SECTION_NAME:
        m_Boxes.PlaySelect();
        switch (m_curSymbol) {
        case 91:
            m_curSymbol = 95;
            break;
        case 95:
            m_curSymbol = 100;
            break;
        case 100:
            m_curSymbol = 78;
            break;
        case 103:
            m_curSymbol = 78;
            break;
        default:
            m_curSymbol++;
            break;
        }
        break;
    case SECTION_AVATAR:
        m_Boxes.PlaySelect();
        m_Avatar--;
        if(m_Avatar < 0) m_Avatar = 39;
        m_textureBase.x = m_Avatar % 4;
        m_textureBase.y = m_Avatar / 4;
        m_textureBase.x *= 96;
        m_textureBase.y *= 128;
        PositionCharacterRing(m_Avatar);
        break;
    case SECTION_RECRUIT:
        break;
    case SECTION_REMOVE:
        break;
    }
}

void Tactics::SelectDown(Engine* e) {
    switch(m_section) {
    case SECTION_NORMAL:
        m_Boxes.PlaySelect();
        ++m_CurrentSelectionY;
        if(m_CurrentSelectionY > 4) m_CurrentSelectionY = 0;
        if(m_CurrentSelectionY != 0 && heroSelected) heroSelected = false;
        break;
    case SECTION_NAME:
        m_Boxes.PlaySelect();
        switch (m_curSymbol) {
        case 66:
        case 67:
        case 68:
            m_curSymbol = 91;
            break;
        case 70:
        case 71:
        case 72:
        case 73:
            m_curSymbol = 95;
            break;
        case 75:
        case 76:
        case 77:
            m_curSymbol = 100;
            break;
        default:
            if(m_curSymbol > 77) m_curSymbol -= 78;
            else m_curSymbol += 26;
            break;
        }
        break;
    case SECTION_AVATAR:
        break;
    case SECTION_RECRUIT:
        if(e->getParty().getSize() == 7) break;
        m_Boxes.PlaySelect();
        ++m_submenu_selection;
        if(m_submenu_selection > 3) m_submenu_selection = 0;
        break;
    case SECTION_REMOVE:
        if(e->getParty().getSize() == 1) break;
        m_Boxes.PlaySelect();
        if(m_submenu_selection == 0) m_submenu_selection = 1;
        else m_submenu_selection = 0;
        break;
    }
}

void Tactics::SelectUp(Engine* e) {
    switch(m_section) {
    case SECTION_NORMAL:
        m_Boxes.PlaySelect();
        --m_CurrentSelectionY;
        if(m_CurrentSelectionY < 0) m_CurrentSelectionY = 4;
        if(m_CurrentSelectionY != 0 && heroSelected) heroSelected = false;
        break;
    case SECTION_NAME:
        m_Boxes.PlaySelect();
        switch (m_curSymbol) {
        case 14:
        case 15:
        case 16:
            m_curSymbol = 91;
            break;
        case 18:
        case 19:
        case 20:
        case 21:
            m_curSymbol = 95;
            break;
        case 23:
        case 24:
        case 25:
            m_curSymbol = 100;
            break;
        default:
            if(m_curSymbol < 26) m_curSymbol += 78;
            else m_curSymbol -= 26;
            break;
        }
        break;
    case SECTION_AVATAR:
        break;
    case SECTION_RECRUIT:
        if(e->getParty().getSize() == 7) break;
        m_Boxes.PlaySelect();
        --m_submenu_selection;
        if(m_submenu_selection < 0) m_submenu_selection = 3;
        break;
    case SECTION_REMOVE:
        if(e->getParty().getSize() == 1) break;
        m_Boxes.PlaySelect();
        if(m_submenu_selection == 0) m_submenu_selection = 1;
        else m_submenu_selection = 0;
        break;
    }
}

void Tactics::DoLetter(int current, Engine* e) {
    switch (current) {
    case 91:    // Erase
        if (!m_TargetName.empty()) m_TargetName.pop_back();
        break;
    case 95:   // Random
        m_TargetName = Tool::generateName();
        break;
    case 100:    // Confirm
        if(!m_TargetName.empty()) {
            e->getParty().getMember(m_CurrentSelectionX).setName(m_TargetName);
            m_Boxes.removeBox("Name");
            m_Boxes.removeBox("Alphabet");
            m_Boxes.removeBox("Instructions");
            m_section = SECTION_NORMAL;
        }
        break;
    default:    // Add Letter
        if (m_TargetName.size() < 14) m_TargetName += m_symbols[current];
        break;
    }
}

void Tactics::PositionCharacterRing(int avatar) {
    const float PI = 3.14159265359;
    const float PRECISION = 0.157;
    const float RADIUS = 800.f;

    float piece = 2.0f * PI / 40;
    float offset = piece * avatar;

    float x, y;
    int n = 0;
    for( float i = 0.0f + offset; i > (-2.0f * PI) + offset; i -= PRECISION ) {
        x = std::sin( i ) * RADIUS;
        y = std::cos( i ) * RADIUS;

        float final_y = ((540 + y) * 0.10) + 700;
        m_chars[n].setPosition(sf::Vector2f(960 + x, final_y));

        float old_value = y;
        float old_min = -800.0f;
        float old_max = 800.0f;
        float new_min = 1.0f;
        float new_max = 5.0f;
        float new_value = ( (old_value - old_min) / (old_max - old_min) ) * (new_max - new_min) + new_min;

        m_chars[n].setScale(new_value, new_value);
        n++;
    }
}

void Tactics::DrawName() {
    Box& box = m_Boxes.addBox("Alphabet", sf::Vector2f(1300, 415), sf::Vector2f(310, 300));
    box.setStyle(Box::BOX_CONVO);
    box.addText("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z", sf::Vector2f(50, 050), 48);
    box.addText("a b c d e f g h i j k l m n o p q r s t u v w x y z", sf::Vector2f(50, 140), 48);
    box.addText(". ? < > [ ] ' \" \\ / ! @ # $ % ^ & * ( ) _   - + { }", sf::Vector2f(50, 230), 48);
    box.addText("0 1 2 3 4 5 6 7 8 9 = | ;  Erase   Random   Confirm", sf::Vector2f(50, 320), 48);

    m_Boxes.addBox("Instructions", sf::Vector2f(370, 90), sf::Vector2f(410, 220), Box::BOX_CONVO);
    m_Boxes.getBox("last").addText("Choose a Name", sf::Vector2f(30, 10), 48);
    m_Boxes.addBox("Name", sf::Vector2f(500, 80), sf::Vector2f(1000, 720), Box::BOX_CONVO);
}

void Tactics::DrawAvatar() {
    m_Boxes.addBox("Avatar", sf::Vector2f(1700, 400), sf::Vector2f(110, 550));
    m_Boxes.getBox("Avatar").addText("Choose a new look for the selected hero.", sf::Vector2f(350, 30), 48);
}

void Tactics::DrawTactics(Engine* e) {
    Box& box = m_Boxes.addBox("Body", sf::Vector2f(1520, 700), sf::Vector2f(200, 150));
    box.addText("Reorder", sf::Vector2f(250, 100), 48);
    box.addText("\tPress enter to select a hero and move them to a new position.", sf::Vector2f(250, 146), 36);

    box.addText("Rename", sf::Vector2f(250, 200), 48);
    box.addText("\tChange the selected hero's name.", sf::Vector2f(250, 246), 36);

    box.addText("Redress", sf::Vector2f(250, 300), 48);
    box.addText("\tChange the appearance of the selected hero.", sf::Vector2f(250, 346), 36);

    box.addText("Recruit", sf::Vector2f(250, 400), 48);
    box.addText("\tAdd a new member to the party.", sf::Vector2f(250, 446), 36);

    box.addText("Remove", sf::Vector2f(250, 500), 48, Box::FONT_NORMAL, sf::Color::Red);
    box.addText("\tRemove this party member.\n\tWarning: This cannot be undone.", sf::Vector2f(250, 546), 36);

    m_Boxes.addBox("Title", sf::Vector2f(240, 100), sf::Vector2f(250, 50));
    m_Boxes.getBox("last").addText("Tactics", sf::Vector2f(10, 5), 60);

    // Characters:
    for (int i = 0; i < e->getParty().getSize(); ++i) {
        std::stringstream ss;
        ss << i;
        m_Boxes.addBox("PartyMember" + ss.str(), sf::Vector2f(100, 100), sf::Vector2f(600 + (150 * i), 80));
    }
    m_Boxes.addBox("Selection", v2f(10, 10), v2f(650, 206), Box::BOX_PLAIN); // Holder for selection description
    Hero& hero = e->getParty().getMember(m_CurrentSelectionX);
    std::stringstream ss;
    ss << "The selected Hero is " << hero.getName() << ", a level " << hero.getLevel() << " " << hero.getClassName(hero.getClass()) << ".";
    m_Boxes.getBox("Selection").addText(ss.str(), v2f(0, 0), 32);
}
