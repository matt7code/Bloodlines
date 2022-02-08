#ifndef CHARACTER_DESIGNER_H
#define CHARACTER_DESIGNER_H

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include "gameengine.h"
#include "gamestate.h"
#include "states/play.h"
#include "tool.h"
#include "hero.h"

class CharacterDesigner : public GameState {
public:
//    enum {CLASS_NONE, CLASS_WARRIOR, CLASS_ROGUE, CLASS_WIZARD, CLASS_CLERIC};
    enum {SECTION_WELCOME, SECTION_NAME, SECTION_CLASS, SECTION_AVATAR, SECTION_DONE};

    void Init(Engine* e);
    void Cleanup(Engine* e);
    void Pause(Engine* e);
    void Resume(Engine* e);
    void HandleEvents(Engine* e);
    void Update(Engine* e);
    void Draw(Engine* e);

    static CharacterDesigner* Instance() {
        return &m_CharacterDesigner;
    }

    void DrawWelcome();
    void DrawName();
    void DrawClass();
    void DrawAvatar();
    void DrawDone();

    void SelectUp();
    void SelectDown();
    void SelectLeft();
    void SelectRight();
    void DoLetter(int current);
    void AnimateWalk(float seconds);
    void PositionCharacterRing(int avatar);

protected:

    CharacterDesigner() { }

private:
    static CharacterDesigner m_CharacterDesigner;
    Boxes               m_Boxes;
    sf::Texture         m_backgroundTexture;
    sf::Sprite          m_background;
    int                 m_section;
    int                 m_curSymbol = 0;
    std::string         m_symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.?<>[]'\"\\/!@#$%^&*()_ -+{}0123456789=|;             ";
    sf::Vector2f        m_arrowPosName;
    int                 m_classChoice = 1;

    std::string         m_TargetName = "";
    int                 m_Class;
    int                 m_Avatar = 0;

    int                 m_animFrameSet[4] = {0, 1, 2, 1};
    int                 m_animFrame = 0;

    float               m_AnimSeconds = 0.0f;
    sf::Time            m_AnimTime;
    sf::Texture         m_characters;
    sf::Sprite          m_sprite;
    std::vector<sf::Sprite> m_chars;
    sf::Vector2i        m_textureBase;

//    struct y_compare
//    {
//        inline bool operator() (const sf::Sprite& s1, const sf::Sprite& s2) const
//        {
//            return (s1.getPosition().y < s2.getPosition().y);
//        }
//    };
};

#endif
