#ifndef Edit_Item_H
#define Edit_Item_H

#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "item.h"

class EditItem : public CGameState
{
public:
    void Init(CGameEngine* e);
    void Cleanup(CGameEngine* e) { SaveItems(e); e->m_RenderWindow.setMouseCursorVisible(false); };
    void Pause(CGameEngine* e) {};
    void Resume(CGameEngine* e) {};
    void HandleEvents(CGameEngine* e);
    void Update(CGameEngine* e);
    void Draw(CGameEngine* e);

    void HandleClick(sf::Vector2i clickPos);
    void UpdateList(CGameEngine* e);
    void SaveItems(CGameEngine* e);

    static EditItem* Instance() { return &m_EditItem; }

protected:
    EditItem() { }

private:
    static EditItem     m_EditItem;
    Boxes               m_Boxes;
    std::map<int, sf::IntRect>
                        m_ClickMap;
    int                 m_list_offset = 0;
    int                 m_currentListSlot = 0;
};

#endif
