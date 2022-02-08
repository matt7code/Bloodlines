#include "states/status_stats.h"

StateStats StateStats::m_StateStats;

void StateStats::Init(Engine* e) {
    e->m_View.reset(sf::FloatRect(0, 0, 1920, 1080));
    e->m_View.zoom(0.5f);
    e->m_RenderWindow.setView(e->m_View);

    m_Boxes.addBox("Body", sf::Vector2f(1520, 700), sf::Vector2f(200, 150));
    FillInBody(e->getParty().getMember(m_CurrentSelectedPartyMember), e->m_RenderWindow);

    m_Boxes.addBox("Status", sf::Vector2f(200, 100), sf::Vector2f(250, 50));
    m_Boxes.getBox("last").addText("Status", sf::Vector2f(10, 5), 60);

    // Characters:
    for (int i = 0; i < e->getParty().getSize(); ++i) {
        std::stringstream ss;
        ss << i;
        m_Boxes.addBox("PartyMember" + ss.str(), sf::Vector2f(100, 100), sf::Vector2f(600 + (150 * i), 80));
    }
    // Center the View
    e->m_View.setCenter(e->getParty().getPos());
    m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());

    m_Boxes.PlayConfirm();

//    e->playMusic(CGameEngine::SONG_STATUS);
}

void StateStats::Cleanup(Engine* e) {
    m_Boxes.getBoxes().clear();
    m_statusSlot = AVATAR_ROW;
}

void StateStats::Pause(Engine* e) {
//    e->pauseMusic();
}

void StateStats::Resume(Engine* e) {
//    e->playMusic(CGameEngine::SONG_STATUS);
}

void StateStats::HandleEvents(Engine* e) {
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
            case sf::Keyboard::Return:
                m_Boxes.PlayCancel();
                e->PopState();
                break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                m_Boxes.PlaySelect();
                SelectRight(m_CurrentSelectedPartyMember, e->getParty().getSize(), e);
                break;

            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                m_Boxes.PlaySelect();
                SelectLeft(m_CurrentSelectedPartyMember, e->getParty().getSize(), e);
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                m_Boxes.PlaySelect();
                SelectUp(m_CurrentSelectedPartyMember, e->getParty().getSize());
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                m_Boxes.PlaySelect();
                SelectDown(m_CurrentSelectedPartyMember, e->getParty().getSize());
                break;

            default:
                break;
            }

            if(m_statusSlot != AVATAR_ROW) {
                switch(m_statusSlot) {
                case 0:
                    m_desc = "The bright-eyed daughter, the quick son; Alphabet of names in a green field; Names in the small tracks of birds; Names lifted from a hat; Or balanced on the tip of the tongue.";
                    break;
                case 1:
                    m_desc = "This is the hero's profession. It encapsulates in a general way what the character has dedicated themselves to pursue. Not militia man who picks up a sword is a warrior, and not every child who steals a piece of candy is a rogue. Class indicates dedicated study.";
                    break;
                case 2:
                    m_desc = "Level indicates skill in a given class. Higher level indicates greater skill. With levels gained comes greater health, reserves of magical energy and increased abilities.";
                    break;
                case 3:
                    m_desc = "Experience is gained by adventure. While defeating enemies in battle is a major source of experience - solving puzzles and completing quests can also teach great lessons.";
                    break;
                case 4:
                    m_desc = "This is the total experienced gained by this hero in the entirety of their life.";
                    break;
                case 5:
                    m_desc = "Health is a measure of the punishment you can take. Strong heroes will have more health. If your health drops to zero, you die. If you die, you can be revived, but if everyone in the party dies, the game is over.";
                    break;
                case 6:
                    m_desc = "Magic is a measure of a heroes reserve of magical power. Highly intelligent heroes will have a larger pool to draw upon. As the power of a spell inscreases, it exhausts more magical energy. This enegry can be used to create, destroy, heal and kill.";
                    break;
                case 7:
                    m_desc = "Strength represents not only physical strength, but general toughness and endurance as well. Has a large impact on potential health. Weather forcing a door, or moving a heavy boulder - muscle counts. The prime requisite for great warriors.";
                    break;
                case 8:
                    m_desc = "Brain power. Intelligence influences general magic resistance, and ability to manipulate magical energies. Smart characters can solve puzzels quickly and often learn everal languages. Wizards and Clerics rely on intellect to master their arcane skills and devotional rights.";
                    break;
                case 9:
                    m_desc = "Agility effects many skills both common and uncommon. Agility is speed, manual dexterity, reflexes, and aim with ranged weapons. Heroes with a the gift of great agility can move and act unnoticed. Rogues value this ability above all.";
                    break;
                case 10:
                    m_desc = "These are the bonuses of various types of attacks the hero may employ.";
                    break;
                case 11:
                    m_desc = "Resistance to physical damage. From swords and clubs, to slips and falls.";
                    break;
                case 12:
                    m_desc = "Magic resistance is a measure of willpower, general strength of the mind, and measures raw resistance to harmful magic.";
                    break;
                case 13:
                    m_desc = "Fire. Lava. Intense heat of any kind.";
                    break;
                case 14:
                    m_desc = "Cold. Ice. Damage from intense cold of any kind.";
                    break;
                case 15:
                    m_desc = "Electrical damage comes mainly from damaging magical attacks, and rarely some eels.";
                    break;
                case 16:
                    m_desc = "Acid. Burning, corrosive attacks come mainly from lizards, snakes, and traps.";
                    break;
                case 20:
                    m_desc = "Defensive bonuses are affected by gear, both magical and mundane, and stats like strength, agility and intellect.";
                    break;
                case 21:
                    m_desc = "Affinity for physical damage. Swords, clubs, axes, hammers, and even fists.";
                    break;
                case 22:
                    m_desc = "Magic resistance is a measure of willpower, general strength of the mind, and measures raw resistance to harmful magic.";
                    break;
                case 23:
                    m_desc = "Fire. Lava. Intense heat of any kind.";
                    break;
                case 24:
                    m_desc = "Cold. Ice. Attacks utilizing intense cold of any kind.";
                    break;
                case 25:
                    m_desc = "Electrical damage comes mainly from damaging magical attacks, and rarely, some eels.";
                    break;
                case 26:
                    m_desc = "Acid. Burning, corrosive attacks come mainly from lizards, snakes, traps, and magic.";
                    break;
                default:
                    m_desc = "No Description";
                    break;
                }
                m_Boxes.getBox("Description").getText().clear();
                m_Boxes.getBox("Description").addText(Tool::FormatLine(m_desc, 64), sf::Vector2f(13, -4), 36);
            }
        }
    }
}
//m_desc = "A sick character will suffer severe stat penalties until healed or rested a long time.";
//m_desc = "A paralysed hero cannot move or defend themselves. They are not dead, but cannot continue the fight until the paralysis is removed.";
//m_desc = "Rot is a severe curse or disease and will hurt the hero badly over time until they are cured or dead.";
//m_desc = "Rarely, a terribly powerful enemy can cause instant death with spells or incredible physical attacks. This is the characters resistance to these dire attacks.";
//m_desc = "Negative status conditions can be far worse than normal cuts and scrapes, and must often be cured with magic, rest, or both.";
//m_desc = "Elemental attacks can cause additional damage, and can additionally be resisted. These resistances are based on intellect to a small degree, however magical spells and items will provide the greatest bonuses to elements.";
//m_desc = "resistance to ranged damage represents a chance to dodge an attack altogether. This could be a thrown dagger, dart or even arrows.";
//m_desc = "Usually temporary, being blinded from a result of magic, or a flash of bright light, will signifigantly reduce a heroes chance to hit with spells and physical attacks.";
//m_desc = "Poison will slowly kill a hero until it is cured or wears off.";
//m_desc = "The hero is asleep. They will awaken if hurt, and proabably after some time if left alone.";
//m_desc = "Confusion will cause a hero to randomly attack targets, often even allies.";
//m_desc = "A posessed character is under the control of the enemy. They are ensourced, unable to exert their own will.";

void StateStats::Update(Engine* e) {
    // Update the Level
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    AnimateWalk(e->getTimeRunning().getElapsedTime().asSeconds() - m_AnimTime.asSeconds());
    m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
}

void StateStats::Draw(Engine* e) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);

    // Draw the Party in the slots
    sf::Sprite slotAvatar;
    for (int i = 0; i < e->getParty().getSize(); ++i) {
        slotAvatar = e->getParty().getMember(i).getSprite();

        sf::Vector2i textureSheetBase = e->getParty().getMember(i).getTextureSheetBase();

        if( i == m_CurrentSelectedPartyMember ) {
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
    if(m_statusSlot != AVATAR_ROW) {
        int y = m_statusSlot % 10;
        int x = m_statusSlot / 10;
        m_Boxes.drawArrow(sf::Vector2f(245 + (x * 500), 278 + (y * 53)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
    }
}

void StateStats::AnimateWalk(float seconds) {
    m_AnimSeconds += seconds;
    if (m_AnimSeconds > 0.25f) {
        m_AnimSeconds = 0.0f;
        m_animFrame++;
        if(m_animFrame == 4) m_animFrame = 0;
    }
}

void StateStats::SelectLeft(int currentPartyMember, int partySize, Engine* e) {
    if (m_statusSlot == AVATAR_ROW) {
        --m_CurrentSelectedPartyMember;
        if (m_CurrentSelectedPartyMember < 0) m_CurrentSelectedPartyMember = partySize - 1;
        FillInBody(e->getParty().getMember(m_CurrentSelectedPartyMember), e->m_RenderWindow);
    } else {
        if(m_statusSlot < 10) m_statusSlot += 20;
        else m_statusSlot -= 10;
    }
    if(m_statusSlot == 17) m_statusSlot = 7;
    if(m_statusSlot == 18) m_statusSlot = 8;
    if(m_statusSlot == 19) m_statusSlot = 9;
    if(m_statusSlot == 27) m_statusSlot = 7;
    if(m_statusSlot == 28) m_statusSlot = 8;
    if(m_statusSlot == 29) m_statusSlot = 9;
}

void StateStats::SelectRight(int currentPartyMember, int partySize, Engine* e) {
    if (m_statusSlot == AVATAR_ROW) {
        ++m_CurrentSelectedPartyMember;
        if (m_CurrentSelectedPartyMember > partySize - 1) m_CurrentSelectedPartyMember = 0;
        FillInBody(e->getParty().getMember(m_CurrentSelectedPartyMember), e->m_RenderWindow);
    } else {
        if(m_statusSlot < 20) m_statusSlot += 10;
        else m_statusSlot -= 20;
    }
    if(m_statusSlot == 17) m_statusSlot = 7;
    if(m_statusSlot == 18) m_statusSlot = 8;
    if(m_statusSlot == 19) m_statusSlot = 9;
    if(m_statusSlot == 27) m_statusSlot = 7;
    if(m_statusSlot == 28) m_statusSlot = 8;
    if(m_statusSlot == 29) m_statusSlot = 9;
}

void StateStats::SelectDown(int currentPartyMember, int partySize) {
    if (m_statusSlot == AVATAR_ROW) {
        m_Boxes.addBox("Description", sf::Vector2f(1200, 200), sf::Vector2f(360, 830));

        switch (currentPartyMember) {
        case 0:
            m_statusSlot = 0;
            break;
        case 1:
        case 2:
        case 3:
            m_statusSlot = 10;
            break;
        default:
            m_statusSlot = 20;
            break;
        }
    } else {
        if(m_statusSlot == 29) m_statusSlot = 20;
        else if(m_statusSlot == 19) m_statusSlot = 10;
        else if(m_statusSlot == 9) m_statusSlot = 0;
        else ++m_statusSlot;
    }

    if(m_statusSlot == 17) m_statusSlot = 10;
    if(m_statusSlot == 27) m_statusSlot = 20;
    //if(m_statusSlot == 14) m_statusSlot = 15;
}

void StateStats::SelectUp(int currentPartyMember, int partySize) {
    if (m_statusSlot == AVATAR_ROW) {
        m_Boxes.addBox("Description", sf::Vector2f(1200, 200), sf::Vector2f(360, 830));

        switch (currentPartyMember) {
        case 0:
            m_statusSlot = 9;
            break;
        case 1:
        case 2:
        case 3:
            m_statusSlot = 16;
            break;
        default:
            m_statusSlot = 26;
            break;
        }
    } else {
        switch (m_statusSlot) {
        case 0:
            m_statusSlot = AVATAR_ROW;
            m_Boxes.removeBox("Description");
            break;
        case 10:
            m_statusSlot = AVATAR_ROW;
            m_Boxes.removeBox("Description");
            break;
        case 20:
            m_statusSlot = AVATAR_ROW;
            m_Boxes.removeBox("Description");
            break;
        default:
            m_statusSlot--;
            break;
        }
    }
}

void StateStats::FillInBody(Hero& hero, sf::RenderWindow& window) {
    Box& box = m_Boxes.getBox("Body");

    box.getText().clear();
    box.addText("\n\nLvl\nTnl\nTxp\nHP\nMP\nStr\nInt\nAgi", sf::Vector2f(60, 80), 48);
    box.addText("Attack:\n Phys\n Magi\n Heat\n Cold\n Elec\n Acid", sf::Vector2f(560, 80), 48);
    box.addText("Defence:\n Phys\n Magi\n Heat\n Cold\n Elec\n Acid", sf::Vector2f(1060, 80), 48);

    std::stringstream ss;

    ss.str("");
    ss << hero.getCurHP() << "/" << hero.getMaxHP();
    std::string heroHealth = ss.str();
    ss.str("");
    ss << hero.getCurMP() << "/" << hero.getMaxMP();
    std::string heroMagic = ss.str();

    ss.str("");
    ss << hero.getName() << "\n"
       << hero.getClassName(hero.getClass()) << "\n"
       << Tool::padNum(hero.getLevel(), 16) << "\n"
       << Tool::padNum(hero.getExpTNL(), 16) << "\n"
       << Tool::padNum(hero.getExp(), 16) << "\n"
       << Tool::pad(heroHealth, 16) << "\n"
       << Tool::pad(heroMagic, 16) << "\n"
       << Tool::padNum(hero.getStr(), 16) << "\n"
       << Tool::padNum(hero.getInt(), 16) << "\n"
       << Tool::padNum(hero.getAgi(), 16);

    box.addText(ss.str(), sf::Vector2f(60, 80), 48);

    ss.str("");
    ss << "\n";
    ss << Tool::padNum(hero.getAtkPhys()) << "\n"
       << Tool::padNum(hero.getAtkMagi()) << "\n"
       << Tool::padNum(hero.getAtkHeat()) << "\n"
       << Tool::padNum(hero.getAtkCold()) << "\n"
       << Tool::padNum(hero.getAtkElec()) << "\n"
       << Tool::padNum(hero.getAtkAcid());

    box.addText(ss.str(), sf::Vector2f(760, 80), 48);

    ss.str("");
    ss << "\n";
    ss << Tool::padNum(hero.getDefPhys()) << "\n"
       << Tool::padNum(hero.getDefMagi()) << "\n"
       << Tool::padNum(hero.getDefHeat()) << "\n"
       << Tool::padNum(hero.getDefCold()) << "\n"
       << Tool::padNum(hero.getDefElec()) << "\n"
       << Tool::padNum(hero.getDefAcid());

    box.addText(ss.str(), sf::Vector2f(1260, 80), 48);
}
