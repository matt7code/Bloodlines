#include "states/edit_item.h"

EditItem EditItem::m_EditItem;

using v2f = sf::Vector2f;
using v2i = sf::Vector2i;

void EditItem::Init(Engine* e) {
    e->pauseMusic();
    e->m_RenderWindow.setView(e->m_ViewBase);
    e->m_RenderWindow.setMouseCursorVisible(true);
    m_Boxes.getBoxes().clear();

    m_ClickMap.clear();

    m_offset = 0;
    m_prevClick = 0;
    m_item   = e->Items[0];
    m_filter = Item::ItemType_None;
    m_mode   = MODE_NONE;
    m_click  = 0;
    m_debug  = false;

    m_Boxes.addBox("Filter",    v2f(400,  32), v2f(50,  66)).addText("Filter: None",    v2f(10, -5), 32);
    m_Boxes.addBox("New",       v2f(400,  32), v2f(50, 980)).addText("Create New Item", v2f(65, -5), 32);
    m_Boxes.addBox("List",      v2f(400, 816), v2f(50, 130));

    for(int i = 0; i < 30; i++) m_ClickMap[i] = sf::IntRect(50, 130 + (i * 27), 400, 26);
    m_ClickMap[30] = sf::IntRect( 50,  66, 400,  32);
    m_ClickMap[31] = sf::IntRect( 50, 980, 400,  32);

    m_Boxes.addBox("Name",        v2f(1384,  32), v2f( 483,  66)).addText("Name", v2f(10, -5), 32);
    m_Boxes.addBox("Description", v2f(1384, 152), v2f( 483, 130)).addText("Description", v2f(10, -5), 32);
    m_Boxes.addBox("EquippedSlot", v2f(430,  32), v2f( 483, 315)).addText("Equipped Slot: ", v2f(10, -5), 32);
    m_Boxes.addBox("ItemType",    v2f( 430,  32), v2f( 958, 315)).addText("Type: ", v2f(10, -5), 32);
    m_Boxes.addBox("ItemSubtype", v2f( 430,  32), v2f(1433, 315)).addText("Subtype: ", v2f(10, -5), 32);
    m_Boxes.addBox("ItemID",      v2f( 320,  32), v2f( 483, 379)).addText("ItemID: ", v2f(10, -5), 32);
    m_Boxes.addBox("Stackable",   v2f( 320,  32), v2f( 837, 379)).addText("Stack Limit: ", v2f(10, -5), 32);
    m_Boxes.addBox("QuestItem",   v2f( 320,  32), v2f(1191, 379)).addText("Quest ID: ", v2f(10, -5), 32);
    m_Boxes.addBox("Cost",        v2f( 320,  32), v2f(1545, 379)).addText("Cost: ", v2f(10, -5), 32);
    m_Boxes.addBox("Value1",      v2f( 320,  32), v2f( 483, 443)).addText("Value 1: ", v2f(10, -5), 32);
    m_Boxes.addBox("Value2",      v2f( 320,  32), v2f( 837, 443)).addText("Value 2: ", v2f(10, -5), 32);
    m_Boxes.addBox("Value3",      v2f( 320,  32), v2f(1191, 443)).addText("Value 3: ", v2f(10, -5), 32);
    m_Boxes.addBox("Value4",      v2f( 320,  32), v2f(1545, 443)).addText("Value 4: ", v2f(10, -5), 32);

    m_ClickMap[33] = sf::IntRect( 483,  66, 1384,  32); /// Name
    m_ClickMap[34] = sf::IntRect( 483, 130, 1384, 152); /// Desc
    m_ClickMap[35] = sf::IntRect( 483, 315,  430,  32); /// Equip
    m_ClickMap[36] = sf::IntRect( 958, 315,  430,  32); /// Type
    m_ClickMap[37] = sf::IntRect(1433, 315,  430,  32); /// Subtype
    m_ClickMap[38] = sf::IntRect( 483, 379,  320,  32); /// ItemID
    m_ClickMap[39] = sf::IntRect( 837, 379,  320,  32); /// Stackable
    m_ClickMap[40] = sf::IntRect(1191, 379,  320,  32); /// Quest
    m_ClickMap[41] = sf::IntRect(1545, 379,  320,  32); /// Cost
    m_ClickMap[42] = sf::IntRect( 483, 443,  320,  32); /// Value1
    m_ClickMap[43] = sf::IntRect( 837, 443,  320,  32); /// Value2
    m_ClickMap[44] = sf::IntRect(1191, 443,  320,  32); /// Value3
    m_ClickMap[45] = sf::IntRect(1545, 443,  320,  32); /// Value4

    m_Boxes.addBox("Tip", v2f(1380, 506), v2f(483, 507));
    m_ClickMap[46] = sf::IntRect(483, 507, 1380, 506);  /// Tip

    UpdateList(e);
}

void EditItem::HandleEvents(Engine* e) {
    std::stringstream ss;
    v2i mousePos;
    sf::Event event;

    while (e->m_RenderWindow.pollEvent(event)) {
        switch(event.type) {

        case sf::Event::KeyPressed: /// Exit
            if(event.key.code == sf::Keyboard::F1) m_debug = !m_debug;
            break;

        case sf::Event::Closed: /// Exit
            e->Quit();
            break;

        case sf::Event::TextEntered:
            if(m_debug) std::cout << "Input: " << event.text.unicode << std::endl;

            if(event.text.unicode == 27) { /// Escape
                m_Boxes.PlayCancel();
                SaveItem(e);
                e->PopState();
            } else if(event.text.unicode == 8) { /// Backspace
                if (!m_input.empty())
                    m_input.resize(m_input.size() - 1);
            } else { /// Add a character to the input
                m_input += static_cast<char>(event.text.unicode);
            }

            if(m_click > Cell_EditBegin && m_click < Cell_EditEnd) {
                if(m_item.itemID != 0) {
                    m_Boxes.getBox(getCellName(m_click)).getText().clear();
                    m_Boxes.getBox(getCellName(m_click)).addText(Tool::FormatLine(m_input, 86), v2f(10, -5), 32);
                }
            }
            break;

        case sf::Event::MouseButtonPressed: /// Mouse Press
            HandleClick(Tool::ConvertClickToScreenPos(v2i(event.mouseButton.x, event.mouseButton.y), e->m_RenderWindow), e);
            break;

        case sf::Event::MouseWheelScrolled: /// Mouse Scroll
            mousePos = Tool::ConvertClickToScreenPos(v2i(event.mouseWheelScroll.x, event.mouseWheelScroll.y), e->m_RenderWindow);
            if(sf::IntRect( 50, 130, 400, 816).contains(mousePos)) {
                m_offset -= event.mouseWheelScroll.delta;
                if (m_offset > (int)m_list.size() - 30) m_offset = (int)m_list.size() - 30;
                if (m_offset < 0) m_offset = 0;
                //std::cout << "m_offset: " << m_offset << std::endl;
                RefreshList();
            }
            break;

        default:
            break;
        }
    }
}

void EditItem::SaveItem(Engine* e) {
    //LOG("Saving Item");
    if(m_item.itemID == 0) return;

    std::stringstream ss;

    ss.str(m_input);
    if(m_prevClick == Cell_Name) {
        m_item.name = ss.str();
        std::cout << "Name Field Updated: " << ss.str() << std::endl;
    }
    if(m_prevClick == Cell_Desc) {
        m_item.desc = ss.str();
        std::cout << "Desc Field Updated: " << ss.str() << std::endl;
    }
    if(m_prevClick == Cell_EquipSlot)   ss >> m_item.equipSlot;
    if(m_prevClick == Cell_ItemType)    ss >> m_item.type;
    if(m_prevClick == Cell_ItemSubtype) ss >> m_item.subtype;
    if(m_prevClick == Cell_ItemID) {
        /* ItemIDs cannot be changed */
    }
    if(m_prevClick == Cell_Stackable)   ss >> m_item.stackable;
    if(m_prevClick == Cell_QuestItem)   ss >> m_item.quest;
    if(m_prevClick == Cell_Cost)        ss >> m_item.cost;
    if(m_prevClick == Cell_Value1)      ss >> m_item.value1;
    if(m_prevClick == Cell_Value2)      ss >> m_item.value2;
    if(m_prevClick == Cell_Value3)      ss >> m_item.value3;
    if(m_prevClick == Cell_Value4)      ss >> m_item.value4;

    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
        if (it->itemID == m_item.itemID) {
            //std::clog << "Item Updated : " << m_item.name << std::endl;
            *it = m_item;
            RefreshList();
        }
    }
    MergeList(e);
}

void EditItem::LoadItem() {
    //LOG("Loading Item");
    m_input.clear();
    if ((m_click + m_offset) < (int)m_list.size())
        m_item = m_list[m_click + m_offset];
}

void EditItem::HandleClick(sf::Vector2i clickPos, Engine * e) {
    Item newItem(e->Items.size());

    m_prevClick = m_click;
    m_click = -1;

    for(auto& cm : m_ClickMap) {
        if (cm.second.contains(clickPos)) m_click = cm.first;
    }

    if(m_debug) std::cout << "Debug Click/PrevClick: " << m_click << "/" << m_prevClick << std::endl;

    if(m_prevClick != m_click) {

        if(m_prevClick > Cell_EditBegin && m_prevClick < Cell_EditEnd)
            (m_Boxes.getBox(getCellName(m_prevClick)).getText().begin() + 1)->setColor(sf::Color::White);

        if(m_click > Cell_EditBegin && m_click < Cell_EditEnd)
            (m_Boxes.getBox(getCellName(m_click)).getText().begin() + 1)->setColor(sf::Color::Green);

        SaveItem(e);
    }

    std::stringstream ss;

    switch(m_click) {
    case Cell_Filter:
        if (m_mode == MODE_FILTER) {
            m_mode = MODE_NONE;
            UpdateList(e);
        } else {
            m_mode = MODE_FILTER;
            ListFilters();
        }
        break;

    case Cell_NewItem:
        newItem.type = m_filter;
        e->Items.push_back(newItem);
        UpdateList(e);
        break;

    case Cell_Name:
        m_input = m_item.name;
        break;

    case Cell_Desc:
        m_input = m_item.desc;
        break;

    case Cell_EquipSlot:
        if (m_mode == MODE_SLOT) {
            m_mode = MODE_NONE;
            if (m_click < 11) m_item.equipSlot = m_click;
            UpdateList(e);
        } else {
            m_mode = MODE_SLOT;
            ListSlots();
        }
        break;

    case Cell_ItemType:
        std::cout << "Type Clicked" << std::endl;
        if (m_mode == MODE_TYPE) {
            std::cout << "Processing m_click: " << m_click << std::endl;
            m_mode = MODE_NONE;
            if (m_click < 10) m_item.type = m_click;
            UpdateList(e);
        } else {
            std::cout << "Listing Types" << std::endl;
            m_mode = MODE_TYPE;
            ListTypes();
        }
        break;

    case Cell_ItemSubtype:
        if (m_mode == MODE_SUBTYPE) {
            m_mode = MODE_NONE;
            if (m_click < 10) m_item.subtype = m_click;
            UpdateList(e);
        } else {
            m_mode = MODE_SUBTYPE;
            ListSubtypes();
        }
        break;

    case Cell_ItemID:
        ss << m_item.itemID;
        break;

    case Cell_Stackable:
        ss << m_item.stackable;
        break;

    case Cell_QuestItem:
        ss << m_item.quest;
        break;

    case Cell_Cost:
        ss << m_item.cost;
        break;

    case Cell_Value1:
        ss << m_item.value1;
        break;

    case Cell_Value2:
        ss << m_item.value2;
        break;

    case Cell_Value3:
        ss << m_item.value3;
        break;

    case Cell_Value4:
        ss << m_item.value4;
        break;

    default:
        if (m_click > -1 && m_click < 30) {
            if(m_mode == MODE_FILTER) {
                if(Item::getTypeName(m_filter) != "Error") m_filter = m_click;
                m_item = e->Items[0];
            }
            if(m_mode == MODE_SLOT) {
                if(Item::getSlotName(m_item.equipSlot) != "Error") m_item.equipSlot = m_click;
            }
            if(m_mode == MODE_TYPE) {
                if(Item::getTypeName(m_item.type) != "Error") m_item.type = m_click;
            }
            if(m_mode == MODE_SUBTYPE) {
                if(Item::getSubtypeName(m_item.subtype) != "Error") m_item.subtype = (m_item.type * 100) + m_click;
            }
            if (m_mode == MODE_NONE) {
                LoadItem();
            } else {
                SaveItem(e);
                m_mode = MODE_NONE;
                UpdateList(e);
            }
            UpdateEditor(e);
        } else {
            std::cerr << "Unhandled Click! " << m_click << std::endl;
        }
        break;
    }

    if (m_click > Cell_Desc && m_click < Cell_EditEnd) m_input = ss.str();
}

void EditItem::UpdateEditor(Engine * e) {
    Item& item = m_item;

    m_Boxes.getBox("Name").getText().clear();
    m_Boxes.getBox("Description").getText().clear();
    m_Boxes.getBox("EquippedSlot").getText().clear();
    m_Boxes.getBox("ItemType").getText().clear();
    m_Boxes.getBox("ItemSubtype").getText().clear();
    m_Boxes.getBox("ItemID").getText().clear();
    m_Boxes.getBox("Stackable").getText().clear();
    m_Boxes.getBox("QuestItem").getText().clear();
    m_Boxes.getBox("Cost").getText().clear();
    m_Boxes.getBox("Value1").getText().clear();
    m_Boxes.getBox("Value2").getText().clear();
    m_Boxes.getBox("Value3").getText().clear();
    m_Boxes.getBox("Value4").getText().clear();

    m_Boxes.getBox("Name").addText(item.name, v2f(10, -5), 32);
    m_Boxes.getBox("Description").addText(Tool::FormatLine(item.desc, 86), v2f(10, -5), 32);

    std::stringstream ss;
    ss << item.itemID << " " << item.stackable << " " << item.quest  << " " << item.cost << " " << item.value1 << " " << item.value2    << " " << item.value3 << " " << item.value4;
    std::string temp;

    m_Boxes.getBox("EquippedSlot").addText("Equipped Slot: " + Item::getSlotName(item.equipSlot), v2f(10, -5), 32);
    m_Boxes.getBox("ItemType").addText("Type: " + Item::getTypeName(item.type), v2f(10, -5), 32);
    m_Boxes.getBox("ItemSubtype").addText("Subtype: " + Item::getSubtypeName(item.subtype), v2f(10, -5), 32);

    ss >> temp;
    m_Boxes.getBox("ItemID").addText("ItemID: " + temp, v2f(10, -5), 32);
    ss >> temp;
    m_Boxes.getBox("Stackable").addText("Stack Limit: " + temp, v2f(10, -5), 32);
    ss >> temp;
    m_Boxes.getBox("QuestItem").addText("Quest ID: " + temp, v2f(10, -5), 32);
    ss >> temp;
    m_Boxes.getBox("Cost").addText("Cost: " + temp, v2f(10, -5), 32);

    ss >> temp;
    m_Boxes.getBox("Value1").addText("Value 1: " + temp, v2f(10, -5), 32);
    ss >> temp;
    m_Boxes.getBox("Value2").addText("Value 2: " + temp, v2f(10, -5), 32);
    ss >> temp;
    m_Boxes.getBox("Value3").addText("Value 3: " + temp, v2f(10, -5), 32);
    ss >> temp;
    m_Boxes.getBox("Value4").addText("Value 4: " + temp, v2f(10, -5), 32);
}

void EditItem::Update(Engine * e) {
}

void EditItem::Draw(Engine * e) {
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    if(m_debug) DebugUI(e);
}

void EditItem::UpdateList(Engine * e) {
    m_list.clear();

    /// Create Filtered List
    if(m_filter == Item::ItemType_None) {
        m_list = e->Items;
    } else {
        for(auto it = e->Items.begin(); it != e->Items.end(); ++it) {
            if((int)it->type == m_filter) m_list.push_back(*it);
        }
    }
//    m_listLength = (int)m_list.size();

    RefreshList();

    m_Boxes.getBox("Filter").getText().clear();
    m_Boxes.getBox("Filter").addText("Filter: " + Item::getTypeName(m_filter), v2f(10, -5), 32);
}

void EditItem::RefreshList() {
    /// Fill in the visual list
    m_Boxes.getBox("List").getText().clear();

    for(int i = m_offset; i < m_offset + 30; ++i) {
        if(i > (int)m_list.size() - 1) break;
        m_Boxes.getBox("List").addText(Tool::Clip(m_list[i].name, 26), v2f(20, (i - m_offset) * 27 - 5), 28);
    }
}

void EditItem::ListFilters() {
    m_Boxes.getBox("List").getText().clear();
    for (int i = 0; i < 10; ++i)
        m_Boxes.getBox("List").addText(Item::getTypeName(i), v2f(20, (i * 27) - 5), 28);
}

void EditItem::ListSlots() {
    m_Boxes.getBox("List").getText().clear();
    for (int i = 0; i < 11; ++i)
        m_Boxes.getBox("List").addText(Item::getSlotName(i), v2f(20, (i * 27) - 5), 28);
}

void EditItem::ListTypes() {
    m_Boxes.getBox("List").getText().clear();
    for (int i = 0; i < 10; ++i)
        m_Boxes.getBox("List").addText(Item::getTypeName(i), v2f(20, (i * 27) - 5), 28);
}

void EditItem::ListSubtypes() {
    m_Boxes.getBox("List").getText().clear();

    switch(m_item.type) {
    case Item::ItemType_None:
        m_Boxes.getBox("List").addText(Item::getSubtypeName(0), v2f(20, -5), 28);
        break;
    case Item::ItemType_Wand:
        for (int i = Item::ItemSubType_Wand; i < Item::ItemSubType_Wand_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 100) * 27) - 5), 28);
        break;
    case Item::ItemType_Potion:
        for (int i = Item::ItemSubType_Potion; i < Item::ItemSubType_Potion_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 200) * 27) - 5), 28);
        break;
    case Item::ItemType_Armor:
        for (int i = Item::ItemSubType_Armor; i < Item::ItemSubType_Armor_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 300) * 27) - 5), 28);
        break;
    case Item::ItemType_Weapon:
        for (int i = Item::ItemSubType_Weapon; i < Item::ItemSubType_Weapon_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 400) * 27) - 5), 28);
        break;
    case Item::ItemType_Scroll:
        for (int i = Item::ItemSubType_Scroll; i < Item::ItemSubType_Scroll_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 500) * 27) - 5), 28);
        break;
    case Item::ItemType_Junk:
        for (int i = Item::ItemSubType_Junk; i < Item::ItemSubType_Junk_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 600) * 27) - 5), 28);
        break;
    case Item::ItemType_Key:
        for (int i = Item::ItemSubType_Key; i < Item::ItemSubType_Key_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 700) * 27) - 5), 28);
        break;
    case Item::ItemType_Tool:
        for (int i = Item::ItemSubType_Tool; i < Item::ItemSubType_Tool_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 800) * 27) - 5), 28);
        break;
    case Item::ItemType_Valuable:
        for (int i = Item::ItemSubType_Valuable; i < Item::ItemSubType_Valuable_End; ++i)
            m_Boxes.getBox("List").addText(Item::getSubtypeName(i), v2f(20, ((i - 900) * 27) - 5), 28);
        break;
    default:
        std::cerr << "Unhandled Subtype" << std::endl;
        break;
    }
}

void EditItem::MergeList(Engine * e) {
    //LOG("Merging List");
    for(auto it = m_list.begin(); it != m_list.end(); ++it) {
        e->Items[it->itemID] = *it;
    }
    //LOG("List Merged");
}
void EditItem::SaveList(Engine * e) {
    MergeList(e);
    LOG("Saving Items to Disk");
    std::ofstream of("items.db");
    if (!of) return;

    of << std::endl;
    of << "# Item ID, stackable, quest, equipslot, type, subtype, cost, value1, value2, value3, value4" << std::endl;
    of << "# Name" << std::endl;
    of << "# Desc" << std::endl;
    of << std::endl;
    of << "# Slots, Types & Subtypes:" << std::endl;
    of << std::endl;
    of << "# Slot_None = 0, Slot_Head = 1, Slot_Neck = 2, Slot_Armor = 3, Slot_Hand_R = 4, Slot_Hand_L = 5," << std::endl;
    of << "# Slot_Ring_R = 6, Slot_Ring_L = 7, Slot_Gloves = 8, Slot_Belt = 9, Slot_Boots = 10" << std::endl;
    of << "# ItemType_None = 0, ItemType_Wand = 1, ItemType_Potion = 2, ItemType_Armor = 3," << std::endl;
    of << "# ItemType_Weapon = 4, ItemType_Scroll = 5, ItemType_Junk = 6, ItemType_Key = 7," << std::endl;
    of << "# ItemType_Tool = 8, ItemType_Valuable = 9};" << std::endl;
    of << "# ItemSubType_None = 0," << std::endl;
    of << "# ItemSubType_Wand = 100,   ItemSubType_Wand_Lightning, ItemSubType_Wand_Missiles," << std::endl;
    of << "# ItemSubType_Wand_Fireball, ItemSubType_Wand_Cold, ItemSubType_Wand_End" << std::endl;
    of << "# ItemSubType_Potion = 200, ItemSubType_Potion_Healing, ItemSubType_Potion_Poison," << std::endl;
    of << "# ItemSubType_Potion_Mana, ItemSubType_Potion_End," << std::endl;
    of << "# ItemSubType_Armor = 300,  ItemSubType_Armor_Head, ItemSubType_Armor_Body, ItemSubType_Armor_Neck," << std::endl;
    of << "# ItemSubType_Armor_Shield, ItemSubType_Armor_Ring, ItemSubType_Armor_Gloves, ItemSubType_Armor_Belt," << std::endl;
    of << "# ItemSubType_Armor_Boots,ItemSubType_Armor_End," << std::endl;
    of << "# ItemSubType_Weapon = 400, ItemSubType_Weapon_Sword, ItemSubType_Weapon_Dagger, ItemSubType_Weapon_Bow," << std::endl;
    of << "# ItemSubType_Weapon_Hammer, ItemSubType_Weapon_2H_Sword, ItemSubType_Weapon_Staff,ItemSubType_Weapon_End," << std::endl;
    of << "# ItemSubType_Scroll = 500, ItemSubType_Scroll_Healing, ItemSubType_Scroll_Teleport," << std::endl;
    of << "# ItemSubType_Scroll_Exit, ItemSubType_Scroll_Light, ItemSubType_Scroll_RemoveCurse,ItemSubType_Scroll_End," << std::endl;
    of << "# ItemSubType_Junk = 600,   ItemSubType_Junk_Large, ItemSubType_Junk_Medium, ItemSubType_Junk_Small," << std::endl;
    of << "# ItemSubType_Junk_End,ItemSubType_Key = 700,    ItemSubType_Key_Magic, ItemSubType_Key_Normal,ItemSubType_Key_End," << std::endl;
    of << "# ItemSubType_Tool = 800,   ItemSubType_Tool_Tent, ItemSubType_Tool_Shovel,ItemSubType_Tool_End," << std::endl;
    of << "# ItemSubType_Valuable = 900,  ItemSubType_Valuable_Gold, ItemSubType_Valuable_Gem," << std::endl;
    of << "# ItemSubType_Valuable_Jewelry,ItemSubType_Valuable_End" << std::endl;
    of << std::endl;

    for(auto& i : e->Items) {
        of << i.itemID << "\t" << i.stackable << "\t" << i.quest << "\t" << i.equipSlot;
        of << "\t" << i.type << "\t" << i.subtype << "\t" << i.cost << "\t" << i.value1;
        of << "\t" << i.value2 << "\t" << i.value3 << "\t" << i.value4 << std::endl;
        of << i.name << std::endl;
        of << i.desc << std::endl;
    }
    of.close();
    LOG("Saved Items to Disk");
}

void EditItem::DebugUI(Engine * e) {
    for(auto it = m_ClickMap.begin(); it != m_ClickMap.end(); ++it) {
        sf::IntRect& r = it->second;
        sf::RectangleShape rect;

        rect.setSize(v2f(r.width, r.height));
        rect.setPosition(r.left, r.top);
        rect.setOutlineColor(sf::Color::Red);
        rect.setOutlineThickness(2);
        rect.setFillColor(sf::Color::Transparent);

        e->m_RenderWindow.draw(rect);
    }
    m_Boxes.getBox("Tip").getText().clear();

    std::stringstream ss;
    ss  << "\nFilter      : " << m_filter
        << "\nprev_click  : " << m_prevClick
        << "\nm_click     : " << m_click
        << "\nm_offset    : " << m_offset
        << "\nitemID      : " << m_item.itemID
        << "\nm_list.size : " << m_list.size()
        << "\nm_input     : \"" << m_input << "\"";

    m_Boxes.getBox("Tip").addText(ss.str(), v2f(10, -5), 36);
}

std::string EditItem::getCellName(int cell) {
    std::string ret = "Error";
    switch (cell) {
    case EditItem::Cell_Name:
        ret = "Name";
        break;
    case EditItem::Cell_Desc:
        ret = "Description";
        break;
    case EditItem::Cell_EquipSlot:
        ret = "EquippedSlot";
        break;
    case EditItem::Cell_ItemType:
        ret = "ItemType";
        break;
    case EditItem::Cell_ItemSubtype:
        ret = "ItemSubtype";
        break;
    case EditItem::Cell_ItemID:
        ret = "ItemID";
        break;
    case EditItem::Cell_Stackable:
        ret = "Stackable";
        break;
    case EditItem::Cell_QuestItem:
        ret = "QuestItem";
        break;
    case EditItem::Cell_Cost:
        ret = "Cost";
        break;
    case EditItem::Cell_Value1:
        ret = "Value1";
        break;
    case EditItem::Cell_Value2:
        ret = "Value2";
        break;
    case EditItem::Cell_Value3:
        ret = "Value3";
        break;
    case EditItem::Cell_Value4:
        ret = "Value4";
        break;
    default:
        std::cerr << "getCellName Error: Unknown Cell" << std::endl;
        ret = "Error";
        break;
    }
    return ret;
}
