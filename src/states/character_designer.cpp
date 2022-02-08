#include "states/character_designer.h"

CharacterDesigner CharacterDesigner::m_CharacterDesigner;

void CharacterDesigner::Init(Engine* e) {
    if (!m_backgroundTexture.loadFromFile("graphics/dark_castle.png"))
        std::cout << "Error loading graphics/dark_castle.png" << std::endl;
    m_background.setTexture(m_backgroundTexture);

    if(!e->getParty().getTexture().loadFromFile("graphics/characters.png"))
        std::cerr << "Problem loading graphics/characters.png" << std::endl;

    m_sprite.setTexture(e->getParty().getTexture());

    for (int i = 0; i != 40; i++) {
        int x = i % 4;
        int y = i / 4;
        m_sprite.setTextureRect(sf::IntRect((x * 96) + 32, y * 128, 32, 32));
        m_sprite.setOrigin(Tool::centerSprite(m_sprite));
        m_sprite.setScale(4.0f, 4.0f);
        m_chars.push_back(m_sprite);
    }
    // View
    sf::View tempView;
    tempView.reset(sf::FloatRect(0, 0, 1920, 1080));
    e->m_RenderWindow.setView(tempView);

    m_Boxes.getBoxes().clear();
    DrawWelcome();

    e->playMusic(Engine::SONG_STATUS);
}

void CharacterDesigner::Cleanup(Engine* e) {
    e->getTimeRunning().restart();
    e->stopMusic();
}

void CharacterDesigner::Pause(Engine* e) {
    e->pauseMusic();
}

void CharacterDesigner::Resume(Engine* e) {
    e->playMusic(Engine::SONG_STATUS);
}

void CharacterDesigner::HandleEvents(Engine* e) {
    bool change = false;
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
                m_Boxes.getBoxes().clear();
                --m_section;
                if (m_section == -1) {
                    m_section = 0;
                    e->PopState();
                    return;
                }
                if (m_section == SECTION_WELCOME)   DrawWelcome();
                if (m_section == SECTION_NAME)      DrawName();
                if (m_section == SECTION_CLASS)     DrawClass();
                if (m_section == SECTION_AVATAR)    DrawAvatar();
                if (m_section == SECTION_DONE)      DrawDone();
                break;

            case sf::Keyboard::Return:
                m_Boxes.PlayConfirm();
                if (m_section == SECTION_WELCOME) {
                    m_Boxes.getBoxes().clear();
                    if (m_curSymbol == 78) m_curSymbol = 80;
                    int x = m_curSymbol % 26;
                    int y = m_curSymbol / 26;
                    m_arrowPosName = sf::Vector2f(355 + (x * 48), 390 + (y * 90));
                    m_Boxes.drawArrow(m_arrowPosName, 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow);
                    m_section = SECTION_NAME;
                    DrawName();
                } else if (m_section == SECTION_NAME) {
                    DoLetter(m_curSymbol);
                    if (m_section == SECTION_CLASS) {
                        m_Boxes.getBoxes().clear();
                        DrawClass();
                    }
                } else if (m_section == SECTION_CLASS) {
                    m_Boxes.getBoxes().clear();
                    m_Class = m_classChoice;
                    DrawAvatar();
                    m_section = SECTION_AVATAR;
                } else if (m_section == SECTION_AVATAR) {
                    m_Boxes.getBoxes().clear();
                    DrawDone();
                    m_section = SECTION_DONE;
                } else if (m_section == SECTION_DONE) {
                    m_Boxes.getBoxes().clear();
                    m_section = 0;
                    e->getParty().AddMember(m_Avatar, 1, m_Class, m_TargetName);
                    e->ChangeState( Play::Instance() );
                }
                break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                if(m_section != SECTION_WELCOME
                        && m_section != SECTION_DONE
                        && m_section != SECTION_CLASS) m_Boxes.PlaySelect();
                SelectRight();
                change = true;
                break;

            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                if(m_section != SECTION_WELCOME
                        && m_section != SECTION_DONE
                        && m_section != SECTION_CLASS) m_Boxes.PlaySelect();
                SelectLeft();
                change = true;
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                if(m_section != SECTION_WELCOME
                        && m_section != SECTION_DONE
                        && m_section != SECTION_AVATAR) m_Boxes.PlaySelect();
                SelectUp();
                change = true;
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                if(m_section != SECTION_WELCOME
                        && m_section != SECTION_DONE
                        && m_section != SECTION_AVATAR) m_Boxes.PlaySelect();
                SelectDown();
                change = true;
                break;
            default:
                break;
            }
        }
    }
    if(change && m_section == SECTION_CLASS) {
        m_Boxes.getBoxes().clear();
        DrawClass();
        m_arrowPosName = sf::Vector2f(1658, 308 + (m_classChoice * 50));
    }
    if(change && m_section == SECTION_NAME) {
        int x = m_curSymbol % 26;
        int y = m_curSymbol / 26;
        m_arrowPosName = sf::Vector2f(355 + (x * 48), 390 + (y * 95));
        if (m_curSymbol == 91) m_arrowPosName = sf::Vector2f(380 + (x * 48), 385 + (y * 95));
        if (m_curSymbol == 95) m_arrowPosName = sf::Vector2f(380 + (x * 48), 385 + (y * 95));
        if (m_curSymbol == 100) m_arrowPosName = sf::Vector2f(355 + (x * 48), 385 + (y * 95));
    }
}

void CharacterDesigner::Update(Engine* e) {
    switch (m_section) {
    case SECTION_WELCOME:
        break;

    case SECTION_NAME:
        m_Boxes.getBox("Name").getText().clear();
        if ((e->getTimeRunning().getElapsedTime().asMilliseconds() % 1000) > 500) {
            m_Boxes.getBox("Name").addText(std::string(m_TargetName.size(), ' ') + '_', sf::Vector2f(20, -2), 64);
        }
        m_Boxes.getBox("Name").addText(m_TargetName, sf::Vector2f(20, -2), 64);
        break;

    case SECTION_CLASS:
        break;

    case SECTION_AVATAR:
        AnimateWalk(e->getTimeRunning().getElapsedTime().asSeconds() - m_AnimTime.asSeconds());
        m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
        m_sprite.setTextureRect(sf::IntRect(m_textureBase.x + (m_animFrameSet[m_animFrame] * TILE_SIZE_X), m_textureBase.y, 32, 32));
        m_sprite.setScale(4.0f, 4.0f);
        m_sprite.setPosition(1560, 278);
        break;

    case SECTION_DONE:
        break;
    }
}

void CharacterDesigner::Draw(Engine* e) {
    e->m_RenderWindow.draw(m_background);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    if (m_section == SECTION_NAME || m_section == SECTION_CLASS) m_Boxes.drawArrow(m_arrowPosName, 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow);
    if (m_section == SECTION_AVATAR) {
        for (const auto& s : m_chars) e->m_RenderWindow.draw(s);
        e->m_RenderWindow.draw(m_sprite);
        m_Boxes.drawArrow(sf::Vector2f(1007, 920), 1, sf::Vector2f(3.0f, 3.0f), e->m_RenderWindow);
    }
}

void CharacterDesigner::DrawWelcome() {
    m_Boxes.addBox("Welcome", sf::Vector2f(1250, 600), sf::Vector2f(335, 235), Box::BOX_CONVO);

    std::string welcome_message1 = "Welcome to Bloodlines";
    std::string welcome_message2 = "You will now create a character which will initially represent you in the world. This character may flourish and become the patron of a strong clan, or die alone. Unlike most traditional RPGs, you will have the opportunity to add and remove potential heroes from your party. Managing the heroes that compose this party, for better or for worse, determines how you will leave you mark on the world.\n";
    std::string welcome_message3 = "Good luck";

    m_Boxes.getBox("Welcome").addText(welcome_message1, sf::Vector2f(430, 60), 36, Box::FONT_NORMAL, sf::Color::Red);
    m_Boxes.getBox("Welcome").addText(Tool::FormatLine(welcome_message2, 67), sf::Vector2f(30, 170), 36);
    m_Boxes.getBox("Welcome").addText(welcome_message3, sf::Vector2f(560, 490), 36, Box::FONT_NORMAL, sf::Color::Yellow);
}

void CharacterDesigner::DrawName() {
    m_Boxes.addBox("Alphabet", sf::Vector2f(1300, 415), sf::Vector2f(310, 300), Box::BOX_CONVO);
    m_Boxes.getBox("Alphabet").addText("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z", sf::Vector2f(50, 050), 48);
    m_Boxes.getBox("Alphabet").addText("a b c d e f g h i j k l m n o p q r s t u v w x y z", sf::Vector2f(50, 140), 48);
    m_Boxes.getBox("Alphabet").addText(". ? < > [ ] ' \" \\ / ! @ # $ % ^ & * ( ) _   - + { }", sf::Vector2f(50, 230), 48);
    m_Boxes.getBox("Alphabet").addText("0 1 2 3 4 5 6 7 8 9 = | ;  Erase   Random   Confirm", sf::Vector2f(50, 320), 48);

    m_Boxes.addBox("Instructions", sf::Vector2f(370, 90), sf::Vector2f(410, 220), Box::BOX_CONVO);
    m_Boxes.getBox("Instructions").addText("Choose a Name", sf::Vector2f(30, 10), 48);

    m_Boxes.addBox("Name", sf::Vector2f(500, 80), sf::Vector2f(1000, 720), Box::BOX_CONVO);
}

void CharacterDesigner::DrawClass() {
    std::string m_classDesc, m_classTitle;

    switch (m_classChoice) {
    case CLASS_WARRIOR:
        m_classTitle = "Warrior";
        m_classDesc = "Questing knights, conquering overlords, royal champions, elite foot soldiers, hardened mercenaries, and bandit kings—as fighters, they all share an unparalleled mastery with weapons and armor, and a thorough knowledge of the skills of combat.";
        break;
    case CLASS_ROGUE:
        m_classTitle = "Rogue";
        m_classDesc = "Rogues rely on skill, stealth, and their foes’ vulnerabilities to get the upper hand in any situation. They have a knack for finding the solution to just about any problem, bringing resourcefulness and versatility to their adventuring parties.";
        break;
    case CLASS_WIZARD:
        m_classTitle = "Wizard";
        m_classDesc = "Drawing on the subtle weave of magic that permeates the cosmos, wizards cast spells of explosive fire, arcing lightning, subtle deception, and brute-force mind control.";
        break;
    case CLASS_CLERIC:
        m_classTitle = "Cleric";
        m_classDesc = "Clerics are intermediaries between the mortal world and the distant planes of the gods. As varied as the gods they serve, they strive to embody the handiwork of their deities.";
        break;
    }

    m_Boxes.Menu({"Warrior", "Rogue", "Wizard", "Cleric"}, sf::Vector2f(1620, 300));

    m_Boxes.addBox("Class", sf::Vector2f(1240, 190), sf::Vector2f(200, 200), Box::BOX_CONVO);
    m_Boxes.getBox("Class").addText(Tool::FormatLine("Class is your character's specialization. It could be thought of like a profession. This will not be able to be changed in your character's life, barring incredibly powerful magic...", 65), sf::Vector2f(50, 30), 36);

    m_Boxes.addBox("ClassDesc", sf::Vector2f(1200, 200), sf::Vector2f(300, 700), Box::BOX_CONVO);
    m_Boxes.getBox("ClassDesc").addText(Tool::FormatLine(m_classDesc, 65), sf::Vector2f(30, 20), 36);

    m_Boxes.addBox("ClassTitle", sf::Vector2f(270, 100), sf::Vector2f(400, 600), Box::BOX_INFO);
    m_Boxes.getBox("ClassTitle").addText(m_classTitle, sf::Vector2f(30, 10), 64);
}

void CharacterDesigner::DrawAvatar() {
    m_Boxes.addBox("Avatar", sf::Vector2f(1440, 160), sf::Vector2f(250, 200), Box::BOX_CONVO);
    m_Boxes.getBox("Avatar").addText(Tool::FormatLine("This is where you choose what your hero will look like. There are ways this can be altered in the future, but it's not common.", 65), sf::Vector2f(50, 30), 36);
}

void CharacterDesigner::DrawDone() {
    m_Boxes.addBox("Done", sf::Vector2f(1240, 260), sf::Vector2f(350, 300), Box::BOX_CONVO);
    m_Boxes.getBox("Done").addText(Tool::FormatLine("You are about to enter the world as " + m_TargetName + " the " + Hero::getClassName(m_Class) + ".", 65), sf::Vector2f(80, 30), 36);
    m_Boxes.getBox("Done").addText(Tool::FormatLine("You can press Cancel (Esc) to go back and change anything you're not sure about, or press Confirm (Enter) to begin your adventures in the world.", 65), sf::Vector2f(45, 100), 36);
}

void CharacterDesigner::SelectUp() {
    if(m_section == SECTION_AVATAR) return;

    if(m_section == SECTION_NAME) {
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
    }
    if(m_section == SECTION_CLASS) {
        m_classChoice--;
        if(m_classChoice < 1) m_classChoice = 4;
    }
}

void CharacterDesigner::SelectDown() {
    if(m_section == SECTION_AVATAR) return;

    if(m_section == SECTION_NAME) {
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
    }
    if(m_section == SECTION_CLASS) {
        m_classChoice++;
        if(m_classChoice > 4) m_classChoice = 1;
    }
}

void CharacterDesigner::SelectLeft() {
    if(m_section == SECTION_AVATAR) {
        m_Avatar++;
        if(m_Avatar > 39) m_Avatar = 0;
        m_textureBase.x = m_Avatar % 4;
        m_textureBase.y = m_Avatar / 4;
        m_textureBase.x *= 96;
        m_textureBase.y *= 128;
        PositionCharacterRing(m_Avatar);
        return;
    }
    if(m_section == SECTION_CLASS) return;

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
}

void CharacterDesigner::SelectRight() {
    if(m_section == SECTION_CLASS) return;
    if(m_section == SECTION_AVATAR) {
        m_Avatar--;
        if(m_Avatar < 0) m_Avatar = 39;
        m_textureBase.x = m_Avatar % 4;
        m_textureBase.y = m_Avatar / 4;
        m_textureBase.x *= 96;
        m_textureBase.y *= 128;
        PositionCharacterRing(m_Avatar);
        return;
    }

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
}

void CharacterDesigner::DoLetter(int current) {
    switch (current) {
    case 91:    // Erase
        if (!m_TargetName.empty()) m_TargetName.pop_back();
        break;
    case 95:   // Random
        m_TargetName = Tool::generateName();
        break;
    case 100:    // Confirm
        if(!m_TargetName.empty()) {
            m_section = SECTION_CLASS;
            m_arrowPosName = sf::Vector2f(1658, 308 + (m_classChoice * 50));
            PositionCharacterRing(m_Avatar);
        }
        break;
    default:    // Add Letter
        if (m_TargetName.size() < 14) m_TargetName += m_symbols[current];
        break;
    }
}

void CharacterDesigner::AnimateWalk(float seconds) {
    m_AnimSeconds += seconds;
    if (m_AnimSeconds > 0.25f) {
        m_AnimSeconds = 0.0f;
        m_animFrame++;
        if(m_animFrame == 4) m_animFrame = 0;
    }
}

void CharacterDesigner::PositionCharacterRing(int avatar) {
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
//    std::sort(m_chars.begin(), m_chars.end(), [](const sf::Sprite& lhs, const sf::Sprite& rhs){ return lhs.getPosition().y < rhs.getPosition().y; });
}
