#include "states/status_inventory.h"

Status_Inventory Status_Inventory::m_Status_Inventory;

using v2f = sf::Vector2f;

void Status_Inventory::Init(Engine* eng) {
    e = eng;
    e->playMusic(Engine::SONG_STATUS);

    std::stringstream ss;
    ss << e->getParty().getPartyGold();
    m_Boxes.addBox("Gold", sf::Vector2f(490, 100), sf::Vector2f(200, 50)).addText("Party Gold\n" + Tool::pad(ss.str(), 16), sf::Vector2f(30, -5), 48, Box::FONT_NORMAL, sf::Color::Yellow);
    m_Boxes.addBox("List", sf::Vector2f(640, 668), sf::Vector2f(50, 182));
    m_Boxes.addBox("Equip", sf::Vector2f(1150, 668), sf::Vector2f(722, 182));

    for (int i = 0; i < e->getParty().getSize(); ++i) {
        m_Boxes.addBox("PartyMember", sf::Vector2f(100, 100), sf::Vector2f(AVATAR_OFFSET_X + (150 * i), AVATAR_OFFSET_Y));
    }
    RefreshList();
    RefreshSlots();
    m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
}

void Status_Inventory::Cleanup(Engine* e) {
    m_Boxes.getBoxes().clear();
    m_List.clear();
    m_partyMember   = 0;
    m_listChoice    = 0;
    m_listOffset    = 0;
    m_mode          = Status_Inventory::MODE_NORMAL;
    m_sub_menu      = 0;
    m_give_target   = 0;
    m_sub_amount    = 0;
}

void Status_Inventory::Pause(Engine* e) {
    e->pauseMusic();
}

void Status_Inventory::Resume(Engine* e) {
    e->playMusic(Engine::SONG_STATUS);
    if(e->getCurrentScript() == "Exit_Inventory") {
        std::cout << "Automatically exiting inventory." << std::endl;
        e->setCurrentScript("");
        e->PopState();
        e->PopState();
    }
}

void Status_Inventory::HandleEvents(Engine* e) {
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
                SelectCancel();
                break;

            case sf::Keyboard::Return:
                //if(!e->getParty().getMember(m_partyMember).getInventory().empty()) SelectConfirm();
                SelectConfirm();
                break;

            case sf::Keyboard::Space:
                if(m_mode == Status_Inventory::MODE_UNEQUIP) {
                    m_Boxes.PlayCancel();
                    m_sub_menu = 0;
                    m_mode = Status_Inventory::MODE_NORMAL;
                } else if(m_mode == Status_Inventory::MODE_NORMAL) {
                    m_Boxes.PlayConfirm();
                    //m_sub_menu = 0;
                    m_mode = Status_Inventory::MODE_UNEQUIP;
                }
                RefreshList();
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                //if(!e->getParty().getMember(m_partyMember).getInventory().empty()) SelectUp();
                SelectUp();
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                //if(!e->getParty().getMember(m_partyMember).getInventory().empty()) SelectDown();
                SelectDown();
                break;

            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                SelectLeft();
                break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                SelectRight();
                break;

            default:
                break;
            }
        }
    }
}

void Status_Inventory::Update(Engine* e) {
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    AnimateWalk(e->getTimeRunning().getElapsedTime().asSeconds() - m_AnimTime.asSeconds());
    m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
}

void Status_Inventory::Draw(Engine* e) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    DrawParty();

    switch(m_mode) {
    case Status_Inventory::MODE_NORMAL:
        if(!e->getParty().getMember(m_partyMember).getInventory().empty())
            m_Boxes.drawArrow(sf::Vector2f(95, 248 + (m_listChoice * 40)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
        break;
    case Status_Inventory::MODE_ITEM_MENU:
        m_Boxes.drawArrow(sf::Vector2f(95, 248 + (m_listChoice * 40)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
        m_Boxes.drawArrow(sf::Vector2f(765, 355 + (m_sub_menu * 44)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow, true);
        break;
    case Status_Inventory::MODE_UNEQUIP:
        m_Boxes.drawArrow(sf::Vector2f(740, 310 + (m_sub_menu * 44)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow, true);
        break;
    case Status_Inventory::MODE_GIVE_TARGET:
        m_Boxes.drawArrow(sf::Vector2f(95, 248 + (m_listChoice * 40)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
        m_Boxes.drawArrow(sf::Vector2f(765, 355 + (m_sub_menu * 44)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow, true);
        m_Boxes.drawArrow(sf::Vector2f(AVATAR_OFFSET_X + 50 + (150 * m_give_target), AVATAR_OFFSET_Y + 120), 1, sf::Vector2f(1.0f, 3.0f), e->m_RenderWindow, true, sf::Color::Yellow);
        break;
    case Status_Inventory::MODE_TRASH_AMOUNT:
        m_Boxes.drawArrow(sf::Vector2f(95, 248 + (m_listChoice * 40)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
        m_Boxes.drawArrow(sf::Vector2f(765, 355 + (m_sub_menu * 44)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow, true);
        break;
    case Status_Inventory::MODE_GIVE_AMOUNT:
        m_Boxes.drawArrow(sf::Vector2f(95, 248 + (m_listChoice * 40)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow);
        m_Boxes.drawArrow(sf::Vector2f(765, 355 + (m_sub_menu * 44)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow, true);
        m_Boxes.drawArrow(sf::Vector2f(AVATAR_OFFSET_X + 50 + (150 * m_give_target), AVATAR_OFFSET_Y + 120), 1, sf::Vector2f(1.0f, 3.0f), e->m_RenderWindow, true, sf::Color::Yellow);
        break;
    }
}

void Status_Inventory::DrawParty() {
    sf::Sprite slotAvatar;
    slotAvatar = e->getParty().getMember(0).getSprite();
    slotAvatar.setOrigin(0, 0);
    slotAvatar.setScale(3.5f, 3.5f);

    for (int i = 0; i < e->getParty().getSize(); ++i) {
        slotAvatar.setPosition(sf::Vector2f(AVATAR_OFFSET_X - 5 + (150 * i), AVATAR_OFFSET_Y - 7));
        sf::Vector2i textureSheetBase = e->getParty().getMember(i).getTextureSheetBase();

        if( i == m_partyMember ) {
            slotAvatar.setTextureRect(sf::IntRect(textureSheetBase.x + (m_animFrameSet[m_animFrame] * TILE_SIZE_X), textureSheetBase.y, 32, 32));
            m_Boxes.drawArrow(sf::Vector2f(AVATAR_OFFSET_X + 50 + (150 * i), AVATAR_OFFSET_Y + 120), 1, sf::Vector2f(1.0f, 3.0f), e->m_RenderWindow, true);
        } else slotAvatar.setTextureRect(sf::IntRect(textureSheetBase.x + 32, textureSheetBase.y, 32, 32));

        e->m_RenderWindow.draw(slotAvatar);
    }
}

void Status_Inventory::AnimateWalk(float seconds) {
    m_AnimSeconds += seconds;
    if (m_AnimSeconds > 0.25f) {
        m_AnimSeconds = 0.0f;
        m_animFrame++;
        if(m_animFrame == 4) m_animFrame = 0;
    }
}

void Status_Inventory::RefreshList() {
    m_List.clear();

    /// Update member description line.
    std::stringstream ss;
    ss << e->getParty().getMember(m_partyMember).getName() << ", a level " << e->getParty().getMember(m_partyMember).getLevel() << " " << e->getParty().getMember(m_partyMember).getClassName(e->getParty().getMember(m_partyMember).getClass()) << ".";
    m_Boxes.getBox("PartyMember").getText().clear();
    m_Boxes.getBox("PartyMember").addText(ss.str(), v2f(100, 150), 45);

    /// Populate List Vector
    for(const auto& i : e->getParty().getMember(m_partyMember).getInventory()) {
        std::string itemName;
        std::string amount;
        std::stringstream ss;
        ss << i.amount;
        ss >> amount;
        std::string entry = Tool::Clip(e->Items[i.type].name, 26);
        entry.resize(28);
        for (int i = 0; i < (int)amount.size(); ++i) {
            entry[entry.size() - (amount.size() - i)] = amount[i];
        }
        //std::cout << "Adding item to list: " << e->Items[i.type].name << " / " << amount << std::endl;
        m_List.push_back(entry);
    }

    /// Populate List Box
    m_Boxes.getBox("List").getText().clear();
    for (int i = m_listOffset; i < m_listOffset + 15; ++i) {
        if(i < (int)m_List.size()) {
            m_Boxes.getBox("List").addText(m_List[i], sf::Vector2f(55, 20 + ((i - m_listOffset) * 40)), 40);
        }
    }

    /// Process Description
    if(m_mode == Status_Inventory::MODE_NORMAL) {
        m_Boxes.removeBox("Description");
        if(e->getParty().getMember(m_partyMember).getInventory().empty()) return;
        int local_index = m_listOffset + m_listChoice;
        int global = e->getParty().getMember(m_partyMember).getInventory()[local_index].type;
        std::string itemDesc = e->Items[global].desc;
        m_Boxes.addBox("Description", sf::Vector2f(1520, 155), sf::Vector2f(200, 883)).addText(Tool::FormatLine(itemDesc, 84), sf::Vector2f(8, -5), 36);
    }
    if(m_mode == Status_Inventory::MODE_UNEQUIP) {
        m_Boxes.removeBox("Description");

        int item_type = e->getParty().getMember(m_partyMember).getItemInSlot(m_sub_menu + 1);
        if(item_type != 0) {
            std::string itemDesc = e->Items[item_type].desc;
            m_Boxes.addBox("Description", sf::Vector2f(1520, 155), sf::Vector2f(200, 883)).addText(Tool::FormatLine(itemDesc, 84), sf::Vector2f(8, -5), 36);
        }
    }
}

void Status_Inventory::SelectLeft() {
    switch(m_mode) {
    case Status_Inventory::MODE_NORMAL:
        if (e->getParty().getSize() == 1) break;
        m_Boxes.PlaySelect();
        --m_partyMember;
        if (m_partyMember < 0) m_partyMember = e->getParty().getSize() - 1;
        m_listChoice = m_listOffset = 0;
        RefreshList();
        RefreshSlots();
        break;
    case Status_Inventory::MODE_ITEM_MENU:
        break;
    case Status_Inventory::MODE_GIVE_TARGET:
        if(e->getParty().getSize() == 1) break;
        m_Boxes.PlaySelect();
        --m_give_target;
        if (m_give_target < 0) m_give_target = e->getParty().getSize() - 1;
        break;
    case Status_Inventory::MODE_TRASH_AMOUNT:
        break;
    case Status_Inventory::MODE_GIVE_AMOUNT:
        break;
    }
}

void Status_Inventory::SelectRight() {
    switch(m_mode) {
    case Status_Inventory::MODE_NORMAL:
        if (e->getParty().getSize() == 1) break;
        m_Boxes.PlaySelect();
        ++m_partyMember;
        if (m_partyMember > e->getParty().getSize() - 1) m_partyMember = 0;
        m_listChoice = m_listOffset = 0;
        RefreshList();
        RefreshSlots();
        break;
    case Status_Inventory::MODE_ITEM_MENU:
        break;
    case Status_Inventory::MODE_GIVE_TARGET:
        if(e->getParty().getSize() == 1) break;
        m_Boxes.PlaySelect();
        ++m_give_target;
        if (m_give_target > e->getParty().getSize() - 1) m_give_target = 0;
        break;
    case Status_Inventory::MODE_TRASH_AMOUNT:
        break;
    case Status_Inventory::MODE_GIVE_AMOUNT:
        break;
    }
}

void Status_Inventory::SelectUp() {
    std::stringstream ss;
    switch(m_mode) {
    case Status_Inventory::MODE_NORMAL:
        if(e->getParty().getMember(m_partyMember).getInventory().size() < 2) return;
        if(m_listChoice > 0) {
            m_Boxes.PlaySelect();
            --m_listChoice;
        } else if (m_listOffset > 0) {
            m_Boxes.PlaySelect();
            --m_listOffset;
        }
        RefreshList();
        break;
    case Status_Inventory::MODE_UNEQUIP:
        m_Boxes.PlaySelect();
        if(--m_sub_menu < 0) m_sub_menu = 9;
        RefreshList();
        break;
    case Status_Inventory::MODE_ITEM_MENU:
        m_Boxes.PlaySelect();
        if(--m_sub_menu < 0) m_sub_menu = 2;
        break;
    case Status_Inventory::MODE_GIVE_TARGET:
        break;
    case Status_Inventory::MODE_TRASH_AMOUNT:
        if(m_sub_amount < e->getParty().getMember(m_partyMember).getInventory()[m_listOffset + m_listChoice].amount) {
            m_Boxes.PlaySelect();
            m_sub_amount++;
        }
        ss.clear();
        ss.str(std::string());
        ss << m_sub_amount;
        m_Boxes.removeBox("TrashAmount");
        m_Boxes.addBox("TrashAmount", sf::Vector2f(270, 64), sf::Vector2f(1150, 350)).addText("How many?  " + ss.str(), sf::Vector2f(10, 0), 40);
        break;
    case Status_Inventory::MODE_GIVE_AMOUNT:
        if(m_sub_amount < e->getParty().getMember(m_partyMember).getInventory()[m_listOffset + m_listChoice].amount) {
            m_Boxes.PlaySelect();
            m_sub_amount++;
        }
        ss.clear();
        ss.str(std::string());
        ss << m_sub_amount;
        m_Boxes.removeBox("GiveAmount");
        m_Boxes.addBox("GiveAmount", sf::Vector2f(270, 64), sf::Vector2f(1150, 350)).addText("How many?  " + ss.str(), sf::Vector2f(10, 0), 40);
        break;
    }
}

void Status_Inventory::SelectDown() {
    std::stringstream ss;
    switch(m_mode) {
    case Status_Inventory::MODE_NORMAL:
        if(e->getParty().getMember(m_partyMember).getInventory().size() < 2) return;
        if(m_listChoice < 14 && m_listChoice < (int)m_List.size() - 1) {
            m_Boxes.PlaySelect();
            m_listChoice++;
        } else if(m_listChoice == 14 && m_listChoice + m_listOffset < (int)m_List.size()) {
            if (m_listOffset < (int)m_List.size() - 15) {
                m_Boxes.PlaySelect();
                ++m_listOffset;
            }
        }
        RefreshList();
        break;
    case Status_Inventory::MODE_ITEM_MENU:
        m_Boxes.PlaySelect();
        if(++m_sub_menu > 2) m_sub_menu = 0;
        break;
    case Status_Inventory::MODE_UNEQUIP:
        m_Boxes.PlaySelect();
        if(++m_sub_menu > 9) m_sub_menu = 0;
        RefreshList();
        break;
    case Status_Inventory::MODE_GIVE_TARGET:
        break;
    case Status_Inventory::MODE_TRASH_AMOUNT:
        if(m_sub_amount > 1) {
            m_Boxes.PlaySelect();
            m_sub_amount--;
        }
        ss.clear();
        ss.str(std::string());
        ss << m_sub_amount;
        m_Boxes.removeBox("TrashAmount");
        m_Boxes.addBox("TrashAmount", sf::Vector2f(270, 64), sf::Vector2f(1150, 350)).addText("How many?  " + ss.str(), sf::Vector2f(10, 0), 40);
        break;
    case Status_Inventory::MODE_GIVE_AMOUNT:
        if(m_sub_amount > 1) {
            m_Boxes.PlaySelect();
            m_sub_amount--;
        }
        ss.clear();
        ss.str(std::string());
        ss << m_sub_amount;
        m_Boxes.removeBox("GiveAmount");
        m_Boxes.addBox("GiveAmount", sf::Vector2f(270, 64), sf::Vector2f(1150, 350)).addText("How many?  " + ss.str(), sf::Vector2f(10, 0), 40);
        break;
    }
}

void Status_Inventory::SelectCancel() {
    switch(m_mode) {
    case Status_Inventory::MODE_NORMAL:
        m_Boxes.PlayCancel();
        e->PopState();
        break;
    case Status_Inventory::MODE_UNEQUIP:
        m_Boxes.PlayCancel();
        m_mode = Status_Inventory::MODE_NORMAL;
        break;
    case Status_Inventory::MODE_ITEM_MENU:
        m_Boxes.PlayCancel();
        m_Boxes.removeBox("ItemMenu");
        m_mode = Status_Inventory::MODE_NORMAL;
        break;
    case Status_Inventory::MODE_GIVE_TARGET:
        m_Boxes.PlayCancel();
        m_Boxes.removeBox("GiveTarget");
        m_mode = Status_Inventory::MODE_ITEM_MENU;
        break;
    case Status_Inventory::MODE_TRASH_AMOUNT:
        m_Boxes.PlayCancel();
        m_Boxes.removeBox("TrashMessage");
        m_Boxes.removeBox("TrashAmount");
        m_mode = Status_Inventory::MODE_ITEM_MENU;
        break;
    case Status_Inventory::MODE_GIVE_AMOUNT:
        m_Boxes.PlayCancel();
        m_Boxes.removeBox("GiveAmount");
        m_mode = Status_Inventory::MODE_GIVE_TARGET;
        break;
    }
}

void Status_Inventory::SelectConfirm() {
    Hero& hero  = e->getParty().getMember(m_partyMember);
    int item;

    if (!hero.getInventory().empty()) {
        item = hero.getInventory()[m_listOffset + m_listChoice].type;
    } else {
        item = -1;
    }

    switch(m_mode) {
    case Status_Inventory::MODE_NORMAL:
        if (item == -1) break;
        m_Boxes.PlayConfirm();
        //m_sub_menu = 0;
        m_mode = Status_Inventory::MODE_ITEM_MENU;
        if(m_sub_menu > 2) m_sub_menu = 0;
        m_Boxes.addBox("ItemMenu", sf::Vector2f(150, 150), sf::Vector2f(750, 325)).addText("Use\nGive\nTrash", sf::Vector2f(40, 0), 40);
        break;

    case Status_Inventory::MODE_UNEQUIP:
        std::cout << "Unequipping Slot Name# " << (m_sub_menu + 1) << " " << Item::getSlotName(m_sub_menu + 1) << std::endl;
        if (hero.getItemInSlot(m_sub_menu + 1) != 0) {
            m_Boxes.PlayCancel();
            hero.invRemove(m_sub_menu + 1, getIsTwoHanded(hero.getItemInSlot(m_sub_menu + 1)));
            RefreshList();
            RefreshSlots();
        } else {
            std::cout << "Can't unequip nothing." << std::endl;
        }
        break;

    case Status_Inventory::MODE_ITEM_MENU:
        m_Boxes.PlayConfirm();
        if(m_sub_menu == 0) {
            switch(e->Items[item].type) {
            case Item::ItemType_Weapon:
                UseWeapon(item, hero);
                break;
            case Item::ItemType_Armor:
                UseArmor(item, hero);
                break;
            case Item::ItemType_Wand:
                std::cout << "Wand used: " << e->Items[item].name << std::endl;
                break;
            case Item::ItemType_Tool:
                std::cout << "Tool used: " << e->Items[item].name << std::endl;
                if(e->Items[item].name == "Tent") {
                    e->setCurrentScript("tent");
                    e->PushState(Action_Cutscene::Instance());
                    hero.invTake(item, 1);
                }
                break;
            case Item::ItemType_Potion:
                std::cout << "Potion used: " << e->Items[item].name << std::endl;
                break;
            case Item::ItemType_Scroll:
                std::cout << "Scroll used: " << e->Items[item].name << std::endl;
                break;
            }

            m_Boxes.removeBox("ItemMenu");
            m_mode = Status_Inventory::MODE_NORMAL;
            RefreshList();
            RefreshSlots();
        } else if(m_sub_menu == 1) {
            //m_give_target = 0;
            m_Boxes.addBox("GiveTarget", sf::Vector2f(160, 64), sf::Vector2f(950, 350)).addText("To who?", sf::Vector2f(8, 0), 40);
            m_mode = Status_Inventory::MODE_GIVE_TARGET;
        } else if(m_sub_menu == 2) {
            if (e->Items[item].quest != 0) m_Boxes.PlayCancel();
            else {
                m_sub_amount = 1;
                m_Boxes.addBox("TrashMessage", sf::Vector2f(160, 64), sf::Vector2f(950, 350)).addText("Trashing", sf::Vector2f(8, 5), 36, Box::FONT_NORMAL, sf::Color::Red);
                m_Boxes.addBox("TrashAmount", sf::Vector2f(270, 64), sf::Vector2f(1150, 350)).addText("How many?  1", sf::Vector2f(10, 0), 40);
                m_mode = Status_Inventory::MODE_TRASH_AMOUNT;
            }
        }
        break;

    case Status_Inventory::MODE_GIVE_TARGET:
        if (m_give_target == m_partyMember) break;
        m_Boxes.PlayConfirm();
        m_sub_amount = 1;
        m_Boxes.addBox("GiveAmount", sf::Vector2f(270, 64), sf::Vector2f(1150, 350)).addText("How many?  1", sf::Vector2f(10, 0), 40);
        m_mode = Status_Inventory::MODE_GIVE_AMOUNT;
        break;

    case Status_Inventory::MODE_GIVE_AMOUNT:
        m_mode = Status_Inventory::MODE_NORMAL;
        item = e->getParty().getMember(m_partyMember).getInventory()[m_listOffset + m_listChoice].type;
        e->getParty().getMember(m_partyMember).invTake(item, m_sub_amount);
        e->getParty().getMember(m_give_target).invGive(item, m_sub_amount);
        if(e->getParty().getMember(m_partyMember).getInventoryAmount(item) == 0) {
            if(m_listChoice > 0) m_listChoice--;
            else if (m_listOffset > 0) --m_listOffset;
        }
        m_Boxes.removeBox("GiveAmount");
        m_Boxes.removeBox("GiveTarget");
        m_Boxes.removeBox("ItemMenu");
        RefreshList();
        break;

    case Status_Inventory::MODE_TRASH_AMOUNT:
        m_mode = Status_Inventory::MODE_NORMAL;
        item = e->getParty().getMember(m_partyMember).getInventory()[m_listOffset + m_listChoice].type;
        e->getParty().getMember(m_partyMember).invTake(item, m_sub_amount);
        if(e->getParty().getMember(m_partyMember).getInventoryAmount(item) == 0) {
            if(m_listChoice > 0) m_listChoice--;
            else if (m_listOffset > 0) --m_listOffset;
        }
        m_Boxes.removeBox("TrashMessage");
        m_Boxes.removeBox("TrashAmount");
        m_Boxes.removeBox("ItemMenu");
        RefreshList();
        break;
    }
}

void Status_Inventory::RefreshSlots() {
    Box& box = m_Boxes.getBox("Equip");
    box.getText().clear();
    box.addText("Head\nNeck\nBody\nHeld\nHeld\nRing\nRing\nHands\nWaist\nFeet\n\nSpacebar to toggle unequip mode.", sf::Vector2f(40, 100), 40);
    for (int i = 0; i < 10; ++i) {
        int item = e->getParty().getMember(m_partyMember).getItemInSlot(i + 1);
        //std::cout << "i + 1" << i + 1 << std::endl;
        if(item != 0 ) box.addText(e->Items[item].name, sf::Vector2f(170, (44 * i) + 100), 40);
    }
}

void Status_Inventory::UseWeapon(int item, Hero& hero) {
    // need to save equip slots in save game
    hero.invEquip(item, e->Items[item].equipSlot, getIsTwoHanded(item), getIsTwoHanded(hero.getItemInSlot(e->Items[item].equipSlot)));

    if(hero.getInventoryAmount(item) == 0) {
        if(m_listChoice > 0) m_listChoice--;
        else if (m_listOffset > 0) --m_listOffset;
    }
}

void Status_Inventory::UseArmor(int item, Hero& hero) {
    hero.invEquip(item, e->Items[item].equipSlot);

    if(hero.getInventoryAmount(item) == 0) {
        if(m_listChoice > 0) m_listChoice--;
        else if (m_listOffset > 0) --m_listOffset;
    }
}

bool Status_Inventory::getIsTwoHanded(int item) {
    return (e->Items[item].subtype == Item::ItemSubType_Weapon_2H_Sword ||
            e->Items[item].subtype == Item::ItemSubType_Weapon_Staff ||
            e->Items[item].subtype == Item::ItemSubType_Weapon_Bow);
}
