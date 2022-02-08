#ifndef Dialog_H
#define Dialog_H

#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "npc.h"
#include "hero.h"
#include "states/combat.h"
#include "states/action_cutscene.h"
#include "states/store.h"

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

class Dialog : public GameState {
public:
    void                Init(Engine* eng);
    void                Cleanup(Engine* e);
    void                Pause(Engine* e) {
        m_paused = true;
    }
    void                Resume(Engine* e) {
        m_paused = false;
    }
    void                HandleEvents(Engine* e);
    void                Update(Engine* e);
    void                Draw(Engine* e);
    static              Dialog* Instance()  {
        return &m_Dialog;
    }

protected:
    Dialog() {}

private:
    enum                {DIALOG_MODE_NONE, DIALOG_MODE_READ, DIALOG_MODE_CHOICE};
    using               vecStr = std::vector<std::string>;
    static              Dialog m_Dialog;
    const int           BOX_CONVO_WIDTH   = 1400;
    const int           BOX_CONVO_HEIGHT  = 204;
    const int           BOX_CONVO_TOP     = 801;
    const int           BOX_CONVO_LEFT    = 260;
    const int           LOOP_LIMIT        = 1000;
    const int           ANNOY_THRESHHOLD  = 15;

    void                ProcessDialog();
    void                ProcessLine();

    void                doCondition(vecStr line);
    void                doChoice(vecStr line);
    void                doGoto(std::string dest);
    void                doGoto(int dest);
    void                doAction(vecStr line);
    void                doStore(vecStr line);

    std::string         getCommand();
    vecStr              getLine();

    std::string         peekCommand(int line_modifier = 0);

    int                 getCurrentLine() {
        return m_cl;
    }
    void                setCurrentLine(int n = 0) {
        m_cl = n;
    }
    void                advanceCurrentLine(int n = 1) {
        m_cl = m_cl + n;
    };

    void                PrintStats();
    void                DebugLine(int cl = -1);
    void                RewindConvo();
    void                AdvanceConvo();
    void                DisplayCurrentBlock();
    void                PrevChoice();
    void                NextChoice();
    void                ConfirmChoice();

    Engine*        e;
    Boxes               m_Boxes;

    vecStr              displayBlock;
    vecStr              SayAnnoy;
    vecStr              SayRandom;

    std::vector<int>    choiceLineRefs;
    std::map<int, vecStr>       dialog;
    std::map<int, vecStr>       ConsequenceMap;
    std::map<std::string, int>  bookmarks;

    Npc*                NpcPtr     = nullptr;
    Hero*               HeroPtr    = nullptr;
    sf::Sprite*         m_npcFace  = nullptr;
    sf::Sprite*         m_pcFace   = nullptr;
    sf::Sprite*         m_currFace = nullptr;

    int                 m_currAnnoyLine = 0;
    int                 m_cl       = 0; /**< Current Internal Dialog Line */
    int                 m_cdl      = 0; /**< Current Display Line */
    int                 m_choice   = 0;
    int                 m_mode     = DIALOG_MODE_READ;
    bool                m_paused   = false;
};

#endif
