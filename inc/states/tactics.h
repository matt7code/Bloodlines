#ifndef TACTICS_STATE_H
#define TACTICS_STATE_H

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "constants.h"
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"
//#include "tool.h"

class Tactics : public GameState {
public:
    enum {SECTION_NORMAL, SECTION_NAME, SECTION_AVATAR, SECTION_RECRUIT, SECTION_REMOVE};

    void                        Init(Engine* e);
    void                        Cleanup(Engine* e);
    void                        Pause(Engine* e);
    void                        Resume(Engine* e);
    void                        HandleEvents(Engine* e);
    void                        Update(Engine* e);
    void                        Draw(Engine* e);
    static                      Tactics* Instance() {
        return &m_TacticsState;
    }

    void                        SelectRight(Engine* e);
    void                        SelectLeft(Engine* e);
    void                        SelectUp(Engine* e);
    void                        SelectDown(Engine* e);
    void                        SelectConfirm(Engine* e);

    void                        AnimateWalk(float seconds);

    void                        DoLetter(int current, Engine* e);
    void                        PositionCharacterRing(int avatar);
    void DrawTactics(Engine* e);
    void DrawName();
    void DrawAvatar();

protected:
    Tactics() {}

private:
    bool                        m_reportMouse;
    sf::Vector2i                m_MousePos;
    int average_level = 0;
    int m_submenu_selection = 0;

    static Tactics              m_TacticsState;
    sf::Time                    m_AnimTime;
    sf::Sprite                  m_Sprite;
    Boxes                       m_Boxes;
    int                         m_CurrentSelectionX = 0;
    int                         m_CurrentSelectionY = 0;
    int                         m_PartySize = 0;
    int                         m_animFrame = 0;
    int                         m_animFrameSet[4] = {0, 1, 2, 1};
    float                       m_AnimSeconds = 0.0f;

    std::vector<sf::Sprite> m_chars;
    bool heroSelected = false;
    std::string m_TargetName;
    int m_Avatar = 0;
    int m_section = SECTION_NORMAL;
    int m_curSymbol = 0;
    std::string m_symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.?<>[]'\"\\/!@#$%^&*()_ -+{}0123456789=|;             ";
    sf::Sprite m_sprite;
    sf::Vector2i m_textureBase;
    sf::Vector2f m_arrowPosName;
};

#endif
