#ifndef Action_Cutscene_H
#define Action_Cutscene_H

#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "gamestate.h"
#include "states/combat.h"
#include "boxes.h"
#include "tool.h"

class Action_Cutscene : public GameState {
public:
    void                    Init(Engine* e);
    void                    Cleanup(Engine* e) {}
    void                    Pause(Engine* e) {}
    void                    Resume(Engine* e) {}
    void                    HandleEvents(Engine* e);
    void                    Update(Engine* e);
    void                    Draw(Engine* e);

    static                  Action_Cutscene* Instance() { return &m_Action_Cutscene; }

protected:
    Action_Cutscene() { }

private:
    using                   vecStr = std::vector<std::string>;
    using                   decVec = std::deque<sf::Vector2f>;
    Engine*                 e;
    Boxes                   m_Boxes;
    sf::View                m_View;
    static Action_Cutscene  m_Action_Cutscene;

    void                    ProcessLine();
    const int               NOTICE_WIDTH    = 1400;
    const int               NOTICE_HEIGHT   = 204;
    const int               NOTICE_TOP      = 801;
    const int               NOTICE_LEFT     = 260;

    const float             SCROLL_SPEED    = 20.0f;

    void                    doMove(float delta);
    void                    doStack(float delta);
    void                    doFade(float delta);
    void                    doUnpause(float delta);
    void                    doScrollText(float delta);

    sf::Time                m_startTime;
    sf::Time                m_pauseTime;
    sf::Time                m_deltaTime;
    vecStr                  m_script;
    std::string             m_scriptName;

    int                     m_cl = 0;

    bool                    m_waiting       = false;
    bool                    m_paused        = false;
    bool                    m_stack         = false;
    float                   m_pauseLength   = 0.0f;

    bool                    m_fading        = false;
    sf::Time                m_fadeStart;
    float                   m_fadeDuration  = 5.0f;
    float                   m_fadeDepthStart = 0.0f; ///< 0 - 100%
    float                   m_fadeDepthFinish = 100.0f; ///< 0 - 100%
    sf::Color               m_fadeColor     = sf::Color::Black;
    sf::RectangleShape      m_full_screen_fade;

    decVec                  m_moveQueue;
    bool                    m_camFollow     = true;
    bool                    m_animateWalk   = true;
    bool                    m_showParty     = true;
    sf::Vector2f            m_prevPos;
    int                     m_prevZ;
    std::vector<sf::Sprite> m_Sprites;
    sf::Texture             m_tempTexture;
    bool                    m_drawLevel;

    vecStr                  m_scrollText;
    bool                    m_scrolling;
    bool                    m_scrollTextLoaded;
    sf::Text                m_scroll;
    sf::Text                m_scrollShadow;
    float                   m_scrollSpeedMod;
};

#endif
