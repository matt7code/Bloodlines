#ifndef STARTSTATE_H
#define STARTSTATE_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"

#include "states/save_load.h"
#include "states/character_designer.h"

class CStartState : public GameState {
public:
    void Init(Engine* Ptr_engine);
    void Cleanup(Engine* Ptr_engine);

    void Pause(Engine* Ptr_engine);
    void Resume(Engine* Ptr_engine);

    void HandleEvents(Engine* Ptr_engine);
    void Update(Engine* Ptr_engine);
    void Draw(Engine* Ptr_engine);

    static CStartState* Instance() {
        return &m_StartState;
    }

    void            UpChoice();
    void            DownChoice();
    void            Select(Engine* Ptr_engine);

protected:
    CStartState() { }

private:
    sf::View                    m_savedView;
    static CStartState          m_StartState;
    sf::Texture                 m_texture;
    sf::Texture                 m_startTexture;
    sf::Sprite                  m_startScreen;
    sf::Sprite                  m_Sword;
    sf::Sprite                  m_Sword2;
    int                         m_currentSelection;
    //sf::Clock                   m_SwordClock;
    //float                       m_SwordRotation;
    Boxes                       m_Boxes;
    int                         m_numberOfTicks;
    float                       m_offset;
    const float                 PI = 3.14159265358;
};

#endif
