#ifndef Combat_H
#define Combat_H

#include <iostream>
#include <vector>
#include <deque>
#include <utility>

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "monster.h"
#include "hero.h"
#include "special.h"
#include "enemy.h"

#include "states/combat_cutscene.h"
#include "status_message.h"
#include "combat_action.h"

class Combat : public GameState {
public:
    void    Cleanup(Engine* eng) { e->stopMusic(); }
    void    Pause(Engine* eng) {}
    void    Resume(Engine* eng);
    void    Init(Engine* eng);
    void    HandleEvents(Engine* eng);
    void    Update(Engine* eng);
    void    Draw(Engine* eng);

    static  Combat* Instance() { return &m_Combat; }

protected:
    Combat() {}

private:
    //static std::vector<float> m_bounceAnim;
    Engine* e;
    enum phase {
        PHASE_NONE,
        PHASE_WIN,
        PHASE_LOSE,
        PHASE_COMMAND,
        PHASE_TARGET,
        PHASE_ATTACK,
        PHASE_DEFEND,
        PHASE_FLEE,
        PHASE_ITEM,
        PHASE_SPELL,
        PHASE_RESULTS
    };

    enum {
        BACKGROUND_NONE,
        BACKGROUND_DUNGEON,
        BACKGROUND_FOREST,
        BACKGROUND_PLAIN,
        BACKGROUND_TOWN,
        BACKGROUND_UNDERWATER,
        BACKGROUND_OCEAN,
        BACKGROUND_CASTLE,
        BACKGROUND_CABIN
    };

    const unsigned int ERROR = 999999999;
    const float TIME_PER_ATTACK_FRAMES_ANIMATION_IN_SECONDS = 0.0166666f; // 30FPS Combat Animation
    const float ATTACK_POWER_PER_LEVEL = 2.5f;
    const int ENEMY_X       = 790;
    const int ENEMY_Y       = 715;
    const int ROW_HEIGHT    = 60;
    const int ROW_WIDTH     = 170;
    const int ROW_OFFSET    = 25;
    const int NUM_ROWS      = 3;

    const int ENEMY_STR = 8;
    const int ENEMY_INT = 8;

    void    ResetValues();

    void    SelectUp();
    void    SelectDown();
    void    SelectLeft();
    void    SelectRight();
    void    SelectConfirm(Engine* e);
    void    SelectCancel(Engine* e);

    void    ScaleArrow(Engine* e);
    void    RefreshInfoBox(Engine* e);
    void    DrawParty(Engine *e);
    void    DrawMonsters(Engine *e);

    void    PlaceCommandArrow(Engine* e);
    void    PlaceTargetArrow(Engine* e);
    void    PlaceActorArrow(Engine* e);
    void    PlaceSubArrow(Engine* e);

    void    DoAttack();
    void    DoSpecial();

    /// Helpers
    void                        ProcessMessages();
    bool                        PartyAllDead();
    bool                        MonstersAllDead();
    void                        EndCombat(Engine* e);

    void                        RotateUp();
    void                        RotateDown();
    void                        RotateLeft();
    void                        RotateRight();

    void                        DoWin(Engine* e);
    void                        DoLose(Engine* e);
    void                        LOGaction();
    bool                        isTargetInParty(Hero& combatant);
    void                        ReportDamage(float damage, int target, std::string desc = " attacks ");
    //float   FigureDamage();

    /// Targeting
    bool                        row1empty();
    bool                        row2empty();
    bool                        row3empty();
    bool                        targetable(int i);

    int                         GetPartyTarget();
    int                         GetFirstLiveHero();
    int                         GetFirstDeadHero();
    int                         GetFirstLiveMonster();
    int                         GetLastLiveHero();
    int                         GetLastLiveMonster();
    int                         GetNextLiveHero();
    int                         GetNextLiveMonster();
    void                        DrawCommandText();

    void                        TakeTurn();
    void                        DoDeath(Hero& combatant);
    void                        AdvanceToActionPhase(Engine* e);

    Boxes                       m_Boxes;
    int                         m_round = 1;
    static Combat               m_Combat;

    sf::Texture                 m_MonstersTex;
    sf::Texture                 m_BackgroundTex;
    sf::Sprite                  m_Background;

    float                       m_difficulty    = 0;
    float                       m_goldMult      = 0;
    float                       m_expMult       = 0;
    bool                        m_canFlee       = true;
    bool                        m_Fleeing       = false;
    bool                        ReportMouse     = false;
    bool                        m_drawDebug = false;
    bool                        m_stanceSwitchPhaseOne = true;

    std::vector<Hero>                                   m_arena;
    std::vector<std::vector<int>>                       m_arrangements;
    std::vector<StatusMessage>                          m_messages;
    std::deque<Action>                                  m_actionQueue;
    std::vector<std::pair<sf::Text, sf::Vector2f>>      m_texts;

    Action                      m_action;
    sf::Time                    m_delta;
    sf::Time                    m_lastTime;
    bool                        m_acceptingInput = true;
    phase                       m_phase = PHASE_COMMAND;

    int                         m_arrowDir = Box::ARROW_RIGHT;
    sf::Vector2f                m_arrowPos = sf::Vector2f(56.0f, 865.0f);
    sf::Vector2f                m_arrowScale = sf::Vector2f(1.5, 1.5f);
    sf::Time                    m_roundStart;
    sf::Vector2f                m_originalActorPos;
    sf::Vector2f                m_originalTargetPos;

    void                        PopulateEnemyParty(std::string potentialEnemies, bool script = false);
    bool                        AllPartyMembersAreDead();
    void                        IncrementKillRecord(std::string tag);
    void                        LoadEnemy(int slot, int id);
    void                        PopulatePositionVector();
    //void                        AnimateNumbers(unsigned dam, sf::Sprite& sprite, sf::Color col, float ratioCompletion, bool dead = false);
    //void                        AnimateWords(std::string word, sf::Sprite& sprite, sf::Color col, float ratioCompletion, bool dead = false);

    sf::Time                    m_resultsTimeStart;
    bool                        m_resultsJustBegan;
    Effect                      m_damage;
    float                       m_total_damage;
    //std::vector<sf::Sprite>     m_effectNumbers;
    std::stringstream           m_strDam;
    phase                       m_prevPhase;
    void                        DoStanceChange(float ratioCompletion);

    //bool                        m_dead;
    void                        EffectsPlace(int actor, std::string effect, sf::Color col);
    void                        EffectsAnimate(float ratioCompletion);
    void                        EffectsDraw();

    void                        SpellDamageAoE(Effect dam, sf::Color col, std::string desc = " attacks ");
    void                        SpellDamage(Effect dam, sf::Color col, std::string desc = " attacks ");
    void                        SpellHeal(bool AoE = false);

};

#endif
