#include "states/edit_item.h"

EditItem EditItem::m_EditItem;

using v2f = sf::Vector2f;
using v2i = sf::Vector2i;

void EditItem::Init(CGameEngine* e)
{
    m_Boxes.getBoxes().clear();
    m_ClickMap.clear();
    m_list_offset = 0;
    m_currentListSlot = 0;

    e->m_RenderWindow.setView(e->m_ViewBase);
    e->pauseMusic();
    e->m_RenderWindow.setMouseCursorVisible(true);

    m_Boxes.addBox("Tp", v2f(400,  32), v2f(50,  66)).addText("Filter: None", v2f(10,-5), 32);
    m_Boxes.addBox("List",   v2f(400, 816), v2f(50, 130));
    m_Boxes.addBox("New",    v2f(400,  32), v2f(50, 980)).addText("Create New Item", v2f(65,-5), 32);

    m_ClickMap[0] = sf::IntRect( 50, 130, 400, 816);
    m_ClickMap[1] = sf::IntRect( 50,  66, 400,  32);
    m_ClickMap[2] = sf::IntRect( 50, 980, 400,  32);

    UpdateList(e);
}

void EditItem::HandleEvents(CGameEngine* e)
{
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event))
    {
        // Close window : exit
        if (event.type == sf::Event::Closed)
        {
            e->Quit();
        }
        if (event.type == sf::Event::KeyPressed)
        {
            switch(event.key.code)
            {
            case sf::Keyboard::Escape:
            case sf::Keyboard::Return:
                m_Boxes.PlayCancel();
                e->PopState();
                break;

            default:
                break;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            HandleClick(Tool::ConvertClickToScreenPos(v2i(event.mouseButton.x, event.mouseButton.y), e->m_RenderWindow));
        }
        if (event.type == sf::Event::MouseWheelScrolled)
        {
            v2i mousePos = Tool::ConvertClickToScreenPos(v2i(event.mouseWheelScroll.x, event.mouseWheelScroll.y), e->m_RenderWindow);
            if(m_ClickMap[0].contains(mousePos)) m_list_offset -= event.mouseWheelScroll.delta;

            if (m_list_offset < 0) m_list_offset = 0;
            if (m_list_offset > (int)e->Items.size() - 30) m_list_offset = e->Items.size() - 30;

            UpdateList(e);
        }
    }
}

void EditItem::HandleClick(sf::Vector2i clickPos)
{
    int target = -1;

    for(auto& cm : m_ClickMap) if (cm.second.contains(clickPos)) target = cm.first;

    switch(target)
    {
        case 0:
            LOG("Middle Section");
            break;

        case 1:
            LOG("Top Section");
            break;

        case 2:
            LOG("Bottom Section");
            break;

        default:
            LOG("Default (No action)");
            break;
    }
}

void EditItem::Update(CGameEngine* e)
{
}

void EditItem::Draw(CGameEngine* e)
{
    m_Boxes.DrawBoxes(e->m_RenderWindow);
}

void EditItem::UpdateList(CGameEngine* e)
{
    m_Boxes.getBox("List").getText().clear();
    for(int i = m_list_offset; i < m_list_offset + 30; ++i)
    {
        if(i > (int)e->Items.size() - 1) break;
        m_Boxes.getBox("List").addText(Tool::Clip(e->Items[i].name, 26), v2f(20, ((i - m_list_offset) * 27) - 5), 28);
    }
}

void EditItem::SaveItems(CGameEngine* e)
{
    std::ofstream of("items.db");
    if (!of) return;

    for(auto& i : e->Items)
    {
        of << i.itemID << "\t" << i.stackable << "\t" << i.quest << "\t" << i.equipSlot;
        of << "\t" << i.type << "\t" << i.subtype << "\t" << i.cost << "\t" << i.value1;
        of << "\t" << i.value2 << "\t" << i.value3 << std::endl;
        of << i.name << std::endl;
        of << i.desc << std::endl;
    }
    of.close();
}










