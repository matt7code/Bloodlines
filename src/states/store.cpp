#include "states/store.h"

Store Store::m_Store;

void Store::Init(Engine* eng) {
    e = eng;
    SetStore();
    std::stringstream ss;
    ss << e->getParty().getPartyGold();
    m_Boxes.addBox("Gold", sf::Vector2f(490, 100), sf::Vector2f(200, 50)).addText("Party Gold\n" + Tool::pad(ss.str(), 16), sf::Vector2f(30, -5), 48, Box::FONT_NORMAL, sf::Color::Yellow);
    m_Boxes.addBox("Inventory", sf::Vector2f(700, 668), sf::Vector2f( 200, 182));
    m_Boxes.addBox("Store", sf::Vector2f(700, 668), sf::Vector2f(1022, 182));
    for (int i = 0; i < e->getParty().getSize(); ++i) m_Boxes.addBox("PartyMember", sf::Vector2f(100, 100), sf::Vector2f(AVATAR_OFFSET_X + (150 * i), AVATAR_OFFSET_Y));

    RefreshChar();
    RefreshStore();
}

void Store::HandleEvents(Engine* e) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            e->Quit();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                m_Boxes.PlayCancel();
                e->PopState();
                break;

            case sf::Keyboard::Space:
            case sf::Keyboard::Return:
                SelectConfirm();
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

void Store::Update(Engine* e) {
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    AnimateWalk(e->getTimeRunning().getElapsedTime().asSeconds() - m_AnimTime.asSeconds());
    m_AnimTime = sf::seconds(e->getTimeRunning().getElapsedTime().asSeconds());
}

void Store::Draw(Engine* e) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->getParty().Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    DrawParty();
    DrawArrows();
}

void Store::Cleanup(Engine* e) {
    m_Boxes.getBoxes().clear();

    m_animFrame     = 0;
    m_AnimSeconds   = 0.0f;

    m_activeBox     = 0;
    m_currChar      = 0;
    m_desc.clear();

    m_StoreInventory.clear();
    m_CharDisplay.clear();
    m_StoreDisplay.clear();
    m_CharOffset    = 0;
    m_CharChoice    = 0;
    m_StoreOffset   = 0;
    m_StoreChoice   = 0;
}

void Store::DrawParty() {
    const int animSet[4] = {0, 1, 2, 1};
    sf::Sprite slotAvatar;
    slotAvatar = e->getParty().getMember(0).getSprite();
    slotAvatar.setOrigin(0, 0);
    slotAvatar.setScale(3.5f, 3.5f);

    for (int i = 0; i < e->getParty().getSize(); ++i) {
        slotAvatar.setPosition(sf::Vector2f(AVATAR_OFFSET_X - 5 + (150 * i), AVATAR_OFFSET_Y - 7));
        sf::Vector2i textureSheetBase = e->getParty().getMember(i).getTextureSheetBase();

        if( i == m_currChar ) {
            slotAvatar.setTextureRect(sf::IntRect(textureSheetBase.x + (animSet[m_animFrame] * TILE_SIZE_X), textureSheetBase.y, 32, 32));
            m_Boxes.drawArrow(sf::Vector2f(AVATAR_OFFSET_X + 50 + (150 * i), AVATAR_OFFSET_Y + 120), 1, sf::Vector2f(1.0f, 3.0f), e->m_RenderWindow, true);
        } else slotAvatar.setTextureRect(sf::IntRect(textureSheetBase.x + 32, textureSheetBase.y, 32, 32));

        e->m_RenderWindow.draw(slotAvatar);
    }
}

void Store::AnimateWalk(float seconds) {
    m_AnimSeconds += seconds;
    if (m_AnimSeconds > 0.25f) {
        m_AnimSeconds = 0.0f;
        m_animFrame++;
        if(m_animFrame == 4) m_animFrame = 0;
    }
}

void Store::SelectConfirm() {
    if (m_activeBox == BOX_CHARACTER) return;
    else if (m_activeBox == BOX_INVENTORY) {
        if (e->Items[e->getParty().getMember(m_currChar).getInventory()[m_CharOffset + m_CharChoice].type].quest == 0)
            SellItem(e->getParty().getMember(m_currChar).getInventory()[m_CharOffset + m_CharChoice].type);
        else m_Boxes.PlayCancel();
    }
    else if (m_activeBox == BOX_STORE) BuyItem(m_StoreInventory[m_StoreOffset + m_StoreChoice]);
}

void Store::SelectLeft() {
    if (m_activeBox == BOX_CHARACTER && e->getParty().getSize() > 1) {
        m_Boxes.PlaySelect();
        if (--m_currChar < 0) m_currChar = e->getParty().getSize() - 1;
        m_CharOffset    = 0;
        m_CharChoice    = 0;
        m_StoreOffset   = 0;
        m_StoreChoice   = 0;
        RefreshChar();
    } else if (m_activeBox == BOX_INVENTORY) {
        m_Boxes.PlaySelect();
        m_activeBox = BOX_STORE;
        RefreshDesc();
    } else if (m_activeBox == BOX_STORE && !m_CharDisplay.empty()) {
        m_Boxes.PlaySelect();
        m_activeBox = BOX_INVENTORY;
        RefreshDesc();
    }
}

void Store::SelectRight() {
    if (m_activeBox == BOX_CHARACTER && e->getParty().getSize() > 1) {
        m_Boxes.PlaySelect();
        if (++m_currChar > e->getParty().getSize() - 1) m_currChar = 0;
        m_CharOffset    = 0;
        m_CharChoice    = 0;
        m_StoreOffset   = 0;
        m_StoreChoice   = 0;
        RefreshChar();
    } else if (m_activeBox == BOX_INVENTORY) {
        m_Boxes.PlaySelect();
        m_activeBox = BOX_STORE;
        RefreshDesc();
    } else if (m_activeBox == BOX_STORE && !m_CharDisplay.empty()) {
        m_Boxes.PlaySelect();
        m_activeBox = BOX_INVENTORY;
        RefreshDesc();
    }
}

void Store::SelectDown() {
    if (m_activeBox == BOX_CHARACTER && !m_CharDisplay.empty()) {
        m_Boxes.PlaySelect();
        m_activeBox = BOX_INVENTORY;
        RefreshDesc();
    } else if (m_activeBox == BOX_CHARACTER && m_CharDisplay.empty()) {
        m_Boxes.PlaySelect();
        m_activeBox = BOX_STORE;
        RefreshDesc();
    } else if (m_activeBox == BOX_INVENTORY) SelectDownInventory();
    else if (m_activeBox == BOX_STORE) SelectDownStore();
}

void Store::SelectUp() {
    if (m_activeBox == BOX_CHARACTER) return;
    else if (m_activeBox == BOX_INVENTORY) SelectUpInventory();
    else if (m_activeBox == BOX_STORE) SelectUpStore();
}

void Store::SelectUpStore() {
    m_Boxes.PlaySelect();
    if(m_StoreChoice > 0) --m_StoreChoice;
    else if (m_StoreOffset > 0) --m_StoreOffset;
    else {
        m_Boxes.removeBox("SaleArrow");
        m_activeBox = BOX_CHARACTER;
    }
    RefreshStore();
}

void Store::SelectUpInventory() {
    m_Boxes.PlaySelect();
    if(m_CharChoice > 0) --m_CharChoice;
    else if (m_CharOffset > 0) --m_CharOffset;
    else {
        m_Boxes.removeBox("SaleArrow");
        m_activeBox = BOX_CHARACTER;
    }
    RefreshChar();
}

void Store::SelectDownStore() {
    if(m_StoreInventory.size() < 2) return;
    if(m_StoreChoice < 14 && m_StoreChoice < (int)m_StoreDisplay.size() - 1) {
        m_Boxes.PlaySelect();
        m_StoreChoice++;
    } else if(m_StoreChoice == 14 && m_StoreChoice + m_StoreOffset < (int)m_StoreDisplay.size()) {
        if (m_StoreOffset < (int)m_StoreDisplay.size() - 15) {
            m_Boxes.PlaySelect();
            ++m_StoreOffset;
        }
    }
    RefreshStore();
}

void Store::SelectDownInventory() {
    if(e->getParty().getMember(m_currChar).getInventory().size() < 2) return;
    if(m_CharChoice < 14 && m_CharChoice < (int)m_CharDisplay.size() - 1) {
        m_Boxes.PlaySelect();
        m_CharChoice++;
    } else if(m_CharChoice == 14 && m_CharChoice + m_CharOffset < (int)m_CharDisplay.size()) {
        if (m_CharOffset < (int)m_CharDisplay.size() - 15) {
            m_Boxes.PlaySelect();
            ++m_CharOffset;
        }
    }
    RefreshChar();
}

void Store::BuyItem(int item) {
    int cost = e->Items[item].cost;
    if(e->getParty().getPartyGold() >= cost) {
        m_Boxes.PlayConfirm();
        e->getParty().modPartyGold(-cost);
        e->getParty().getMember(m_currChar).invGive(item, 1);
    } else {
        m_Boxes.PlayCancel();
    }
    RefreshChar();
    RefreshGold();
}

void Store::SellItem(int item) {
    m_Boxes.PlayConfirm();
    e->getParty().getMember(m_currChar).invTake(item, 1);
    e->getParty().modPartyGold(e->Items[item].cost);
    RefreshChar();
    RefreshGold();
    if(e->getParty().getMember(m_currChar).getInventoryAmount(item) == 0) SelectUp();
}

void Store::SetStore() {
    m_StoreInventory.clear();
    std::string type = e->getCurrentScript();
    if(type == "Weapons") {
        for(const auto& i : e->Items) {
            if (i.type == Item::ItemType_Weapon) m_StoreInventory.push_back(i.itemID);
        }
    } else if(type == "Armor") {
        for(const auto& i : e->Items) {
            if (i.type == Item::ItemType_Armor) m_StoreInventory.push_back(i.itemID);
        }
    } else if(type == "Magic") {
        for(const auto& i : e->Items) {
            if (i.type == Item::ItemType_Wand) m_StoreInventory.push_back(i.itemID);
            if (i.type == Item::ItemType_Potion) m_StoreInventory.push_back(i.itemID);
            if (i.type == Item::ItemType_Scroll) m_StoreInventory.push_back(i.itemID);
        }
    } else if(type == "General") {
        for(const auto& i : e->Items) {
            //if (i.type == Item::ItemType_None) m_StoreInventory.push_back(i.itemID);
            if (i.type == Item::ItemType_Junk) m_StoreInventory.push_back(i.itemID);
            if (i.type == Item::ItemType_Tool) m_StoreInventory.push_back(i.itemID);
        }
    } else if(type == "Valuable") {
        for(const auto& i : e->Items) {
            if (i.type == Item::ItemType_Valuable) m_StoreInventory.push_back(i.itemID);
        }
    } else if(type == "Everything") {
        for(const auto& i : e->Items) {
            if (i.type != Item::ItemSubType_None) m_StoreInventory.push_back(i.itemID);
        }
    } else {
        std::cerr << "Unknown store type: " << type << std::endl;
        e->PopState();
    }
    std::cout << m_StoreInventory.size() << " store items loaded." << std::endl;
}

void Store::RefreshChar() {
    m_CharDisplay.clear();
    /// Populate Character Inventory Vector
    for(const auto& i : e->getParty().getMember(m_currChar).getInventory()) {
        std::string itemName;
        std::string amount;
        std::stringstream ss;
        ss << i.amount;
        ss >> amount;
        std::string entry = Tool::Clip(e->Items[i.type].name, 26);
        entry.resize(28);
        for (int i = 0; i < (int)amount.size(); ++i) entry[entry.size() - (amount.size() - i)] = amount[i];
        m_CharDisplay.push_back(entry);
    }

    /// Display Inventory
    m_Boxes.getBox("Inventory").getText().clear();
    for (int i = m_CharOffset; i < m_CharOffset + 15; ++i) {
        if(i < (int)m_CharDisplay.size()) {
            m_Boxes.getBox("Inventory").addText(m_CharDisplay[i], sf::Vector2f(55, 20 + ((i - m_CharOffset) * 40)), 40);
        }
    }
    RefreshDesc();
}

void Store::RefreshStore() {
    m_StoreDisplay.clear();
    /// Populate Store Vector
    for(auto& i : m_StoreInventory) {
        std::string itemName;
        std::string cost;
        std::stringstream ss;
        ss << e->Items[i].cost;
        ss >> cost;
        std::string entry = Tool::Clip(e->Items[i].name, 26);
        entry.resize(28);
        for (int i = 0; i < (int)cost.size(); ++i) entry[entry.size() - (cost.size() - i)] = cost[i];
        m_StoreDisplay.push_back(entry);
    }

    /// Display Store
    m_Boxes.getBox("Store").getText().clear();
    for (int i = m_StoreOffset; i < m_StoreOffset + 15; ++i) {
        if(i < (int)m_StoreDisplay.size()) {
            m_Boxes.getBox("Store").addText(m_StoreDisplay[i], sf::Vector2f(55, 20 + ((i - m_StoreOffset) * 40)), 40);
        }
    }
    RefreshDesc();
}

void Store::RefreshDesc() {
    m_Boxes.removeBox("Desc");

    if(m_activeBox == BOX_INVENTORY) {
        int global = e->getParty().getMember(m_currChar).getInventory()[m_CharOffset + m_CharChoice].type;
        std::string itemDesc = e->Items[global].desc;
        m_Boxes.addBox("Desc", sf::Vector2f(1520, 155), sf::Vector2f(200, 883)).addText(Tool::FormatLine(itemDesc, 84), sf::Vector2f(8, -5), 36);
        DrawSaleArrow(e->Items[global].name, e->Items[global].cost);
    } else if(m_activeBox == BOX_STORE) {
        int global = m_StoreInventory[m_StoreOffset + m_StoreChoice];
        std::string itemDesc = e->Items[global].desc;
        m_Boxes.addBox("Desc", sf::Vector2f(1520, 155), sf::Vector2f(200, 883)).addText(Tool::FormatLine(itemDesc, 84), sf::Vector2f(8, -5), 36);
        DrawSaleArrow(e->Items[global].name, e->Items[global].cost);
    } else if(m_activeBox == BOX_CHARACTER) {
        std::stringstream ss;
        Hero& hero = e->getParty().getMember(m_currChar);
        ss << hero.getName() << ", a level " << hero.getLevel() << " " << hero.getClassName(hero.getClass()) << ".";
        m_Boxes.addBox("Desc", sf::Vector2f(1520, 155), sf::Vector2f(200, 883)).addText(ss.str(), sf::Vector2f(10, 0), 45);
    }
}

void Store::DrawArrows() {
    if (m_activeBox == Store::BOX_INVENTORY) {
        m_Boxes.drawArrow(sf::Vector2f( 875, 225 + (m_CharChoice  * 40)), 4, sf::Vector2f(1.25, 2.0), e->m_RenderWindow, true);
    } else if (m_activeBox == Store::BOX_STORE) {
        m_Boxes.drawArrow(sf::Vector2f(1045, 225 + (m_StoreChoice * 40)), 2, sf::Vector2f(1.25, 2.0), e->m_RenderWindow, true);
    }
}

void Store::DrawSaleArrow(std::string itemName, int cost) {
    m_Boxes.removeBox("SaleArrow");
    std::stringstream ss;
    if (m_activeBox == Store::BOX_INVENTORY) {
        ss << "Sell" << std::endl << cost << "g";
        m_Boxes.addBox("SaleArrow", sf::Vector2f(100, 100), sf::Vector2f(910, 178 + (m_CharChoice  * 40)));
    }
    if (m_activeBox == Store::BOX_STORE) {
        ss << "Buy" << std::endl << cost << "g";
        m_Boxes.addBox("SaleArrow", sf::Vector2f(100, 100), sf::Vector2f(910, 178 + (m_StoreChoice  * 40)));
    }
    m_Boxes.getBox("last").addText(ss.str(), sf::Vector2f(10,0), 36);
}

void Store::RefreshGold() {
    std::stringstream ss;
    ss << e->getParty().getPartyGold();
    m_Boxes.getBox("Gold").getText().clear();
    m_Boxes.getBox("Gold").addText("Party Gold\n" + Tool::pad(ss.str(), 16), sf::Vector2f(30, -5), 48, Box::FONT_NORMAL, sf::Color::Yellow);
}
