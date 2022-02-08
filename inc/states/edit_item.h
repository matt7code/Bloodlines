#ifndef Edit_Item_H
#define Edit_Item_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "item.h"

class EditItem : public GameState {
public:
    enum Mode {MODE_NONE, MODE_FILTER, MODE_SLOT, MODE_TYPE, MODE_SUBTYPE};

    enum {
        Cell_None,
        Cell_Filter     = 30,
        Cell_NewItem    = 31,
        Cell_EditBegin  = 32,
        Cell_Name, Cell_Desc, Cell_EquipSlot, Cell_ItemType, Cell_ItemSubtype, Cell_ItemID, Cell_Stackable,
        Cell_QuestItem, Cell_Cost, Cell_Value1, Cell_Value2, Cell_Value3, Cell_Value4, Cell_EditEnd, Cell_Tip
    };

    void Init(Engine* e);
    void Cleanup(Engine* e) {
        SaveList(e);
        e->m_RenderWindow.setMouseCursorVisible(false);
    };
    void Pause(Engine* e) {};
    void Resume(Engine* e) {};
    void HandleEvents(Engine* e);
    void Update(Engine* e);
    void Draw(Engine* e);

    void HandleClick(sf::Vector2i clickPos, Engine* e);

    void ListFilters();
    void ListSlots();
    void ListTypes();
    void ListSubtypes();

    void SaveList(Engine* e);
    void MergeList(Engine* e);

    void SaveItem(Engine* e);
    void LoadItem();

    void UpdateList(Engine* e);
    void UpdateEditor(Engine* e);

    //void ScrollList();
    void RefreshList();

    void DebugUI(Engine* e);
    std::string getCellName(int cell);
    static EditItem* Instance() {
        return &m_EditItem;
    }

protected:
    EditItem() { }

private:
    static EditItem     m_EditItem;
    Boxes               m_Boxes;
    std::map<int, sf::IntRect>
    m_ClickMap;
    Mode                m_mode = MODE_NONE;
    std::vector<Item>   m_list;
    Item                m_item;
    int                 m_filter;
    int                 m_offset = 0;
    int                 m_click;
    int                 m_prevClick;
    std::string         m_input;
    bool                m_debug;

    int m_itemListedName = 0;
};

#endif
