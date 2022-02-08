#ifndef Combat_Cutscene_H
#define Combat_Cutscene_H

#include "boxes.h"
#include "tool.h"
#include "gamestate.h"
#include "status_message.h"
#include "combat_action.h"
#include "hero.h"
#include "prop.h"

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

class Combat_Cutscene : public GameState {
public:
    void    Pause(Engine* eng) {}
    void    Resume(Engine* eng) {}

    void    Cleanup(Engine* eng);
    void    Init(Engine* eng);
    void    HandleEvents(Engine* eng);
    void    Update(Engine* eng);
    void    Draw(Engine* eng);

    static  Combat_Cutscene* Instance() {
        return &m_Combat_Cutscene;
    }

protected:
    Combat_Cutscene() {}

private:
    const unsigned                  PROP_LIMIT = 100;
    Engine* e;
    static Combat_Cutscene          m_Combat_Cutscene;

    /// Combat State Information
    std::vector<Hero>*              m_pArena;
    Boxes*                          m_pBoxes;
    sf::Sprite*                     m_pBackground;
    std::vector<StatusMessage>*     m_pMessages;
    Action*                         m_pAction;
    Hero*                           m_pActor;
    Hero*                           m_pTarget;

    /// Spell Resources from Engine
    std::map<std::string, std::vector<std::string>>*    m_pSpells;
    std::vector<sf::Sprite>*                            m_pSprites;
    std::map<std::string, std::vector<sf::Vector2f>>*   m_pAnims;
    std::vector<sf::Sound>*                             m_pSounds;

    void        ProcessLine();

    void        DrawParty();
    void        DrawMonsters();
    void        ProcessMessages();
    void        DrawDebug();
    void        InitializeValues(Engine * eng);
    void        LoadScript();
    void        LoadProps();
    unsigned    CalculateAnimationIndex(Prop& prop);
    void        UpdateAnimatedProp(Prop& prop);

    sf::Time    m_TimeDelta;
    sf::Time    m_TimePrevious;
    sf::Time    m_TimeInPhase;
    sf::Time    m_TimeWait;
    sf::Time    m_TimeLine;

    sf::Vector2f m_originalTargetPos;
    sf::Vector2f m_originalActorPos;

    std::vector<std::string>    m_script;
    std::string                 m_scriptName;

    int                         m_cl;
    bool                        m_debug;
    Prop*                       m_pActiveProp;
    //std::vector<Prop>           m_Props;
    std::map<int, Prop>         m_Props;
    float                       m_waitLength;
    bool                        m_waiting;
    bool                        m_waitingAnim;
    const std::vector<unsigned> m_foeOrder = {2,5,8,11,14,0,3,6,9,12,1,4,7,10,13};
};

#endif
