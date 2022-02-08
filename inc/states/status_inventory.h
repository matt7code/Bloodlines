#ifndef Status_Inventory_STATE_H
#define Status_Inventory_STATE_H

#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

#include "states/action_cutscene.h"

class Status_Inventory : public GameState {
public:
    void    Init(Engine* eng);
    void    Cleanup(Engine* e);
    void    Pause(Engine* e);
    void    Resume(Engine* e);
    void    HandleEvents(Engine* e);
    void    Update(Engine* e);
    void    Draw(Engine* e);
    static  Status_Inventory* Instance() {
        return &m_Status_Inventory;
    }

protected:
    Status_Inventory() { }

private:
    void        SelectRight();
    void        SelectLeft();
    void        SelectUp();
    void        SelectDown();
    void        SelectConfirm();
    void        SelectCancel();

    void        AnimateWalk(float seconds);
    void        RefreshList();
    void        DisplayList();
    void        DrawParty();
    void        RefreshSlots();

    void        UseWeapon(int item, Hero& hero);
    void        UseArmor(int item, Hero& hero);

    bool        getIsTwoHanded(int item);

    enum {MODE_NORMAL, MODE_UNEQUIP, MODE_ITEM_MENU, MODE_GIVE_TARGET, MODE_GIVE_AMOUNT, MODE_TRASH_AMOUNT};
    Engine* e;
    Boxes       m_Boxes;
    static Status_Inventory
    m_Status_Inventory;
    sf::Sprite  m_Sprite;

    int         m_partyMember   = 0;
    int         m_animFrame     = 0;
    int         m_animFrameSet[4] = {0, 1, 2, 1};
    float       m_AnimSeconds   = 0.0f;
    sf::Time    m_AnimTime;
    const int   AVATAR_OFFSET_X = 722;
    const int   AVATAR_OFFSET_Y = 50;

    int         m_listOffset    = 0; // Current Display Line
    int         m_listChoice    = 0; // Current Item Selected
    std::vector<std::string>    m_List;
    std::string m_desc;

    int         m_mode          = MODE_NORMAL;
    int         m_sub_menu      = 0;
    int         m_give_target   = 0;
    int         m_sub_amount    = 0;
    //std::vector<int> m_equipped_slots;
};

#endif
