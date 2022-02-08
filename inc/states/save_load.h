#ifndef SaveLoad_H
#define SaveLoad_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "states/play.h"
#include "gameengine.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "special.h"
#include "party.h"
#include "hero.h"

struct SaveGame {
    std::vector<std::vector<std::string>>
                                       m_savedParty;           /**< Records of every member the party has ever had. */
    std::vector<std::string>
    m_characterRecord;      /**< Lines of text read in from the same file. */
    sf::Texture             m_screenShot;           /**< Screenshot of Savegame */
    std::string             m_date;
    long                    m_timePlayed;
    std::string             m_place;
};

class SaveLoad : public GameState {
public:
    void                    Init(Engine* e);
    void                    Cleanup(Engine* e);
    void                    Pause(Engine* e);
    void                    Resume(Engine* e);
    void                    HandleEvents(Engine* e);
    void                    Update(Engine* e);
    void                    Draw(Engine* e);

    static                  SaveLoad* Instance() { return &m_SaveLoad; }

protected:
    SaveLoad() {}

private:
    bool                    m_SaveLoadMode;
    int                     m_currChoice;
    static SaveLoad         m_SaveLoad;

    void                    Load(int slot, Engine* e);
    void                    Save(int slot, Engine* e);

    void                    SelectConfirm(int choice, Engine* e);
    void                    SelectUp();
    void                    SelectDown();
    void                    SaveScreenShot(int slot, Engine* e);
    void                    LoadScreenie();

    std::vector<std::string> m_GameLabels;
    std::vector<std::vector<std::string>>
                            m_GameRecords;
    Boxes                   m_Boxes;
    sf::View                m_View;
    sf::Texture             m_screenShotTex;
    sf::Texture             m_currGameTex;
    sf::Image               m_screenShotDisk;
    bool                    m_takeScreenShot = false;
    sf::Sprite              m_screenShotSprite;
    int number_of_saves = 0;
    bool                    m_loading = false;
};

#endif
