#ifndef Store_STATE_H
#define Store_STATE_H

#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "hero.h"

class Store : public GameState {
public:
    void            Init(Engine* e);
    void            Cleanup(Engine* e);
    void            Pause(Engine* e) {};
    void            Resume(Engine* e) {};
    void            HandleEvents(Engine* e);
    void            Update(Engine* e);
    void            Draw(Engine* e);
    static          Store* Instance() {
        return &m_Store;
    }

protected:
    Store() {}

private:
    void            SelectRight();
    void            SelectLeft();
    void            SelectUp();
    void            SelectDown();
    void            SelectConfirm();

    void            DrawParty();
    void            DrawArrows();
    void            DrawSaleArrow(std::string itemName, int cost);
    void            AnimateWalk(float seconds);

    void            SelectUpStore();
    void            SelectUpInventory();
    void            SelectDownStore();
    void            SelectDownInventory();

    void            SetStore();
    void            SellItem(int item);
    void            BuyItem(int item);

    void            RefreshChar();
    void            RefreshStore();
    void            RefreshDesc();
    void            RefreshGold();

    const int       AVATAR_OFFSET_X = 722;
    const int       AVATAR_OFFSET_Y = 50;
    const int       BOX_CHARACTER   = 0;
    const int       BOX_INVENTORY   = 1;
    const int       BOX_STORE       = 2;

    Engine*    e;
    static Store    m_Store;
    Boxes           m_Boxes;

    int             m_animFrame     = 0;
    float           m_AnimSeconds   = 0.0f;
    sf::Time        m_AnimTime;

    int             m_activeBox     = 0;
    int             m_currChar      = 0;
    std::string     m_desc;

    std::vector<int> m_StoreInventory;
    std::vector<std::string> m_CharDisplay;
    std::vector<std::string> m_StoreDisplay;
    int             m_CharOffset    = 0;
    int             m_CharChoice    = 0;
    int             m_StoreOffset   = 0;
    int             m_StoreChoice   = 0;
};

#endif
