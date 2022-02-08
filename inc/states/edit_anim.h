#ifndef Edit_Anim_H
#define Edit_Anim_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <set>

#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "input_string.h"

class Edit_Anim : public GameState {
public:
    enum    {GRID_OFF, GRID_TINY, GRID_MEDIUM, GRID_LARGE, GRID_LARGER, GRID_HUGE, GRID_MASSIVE, GRID_OMG_SO_BIG, GRID_FAINTS, GRID_MAX};
    enum    {MODE_NONE, MODE_FPS, MODE_SAVE, MODE_LOAD};

    void    Init(Engine* engine);
    void    HandleEvents(Engine* engine);
    void    Update(Engine* engine);
    void    Draw(Engine* engine);
    void    Cleanup(Engine* engine);
    void    Pause(Engine* engine) {};
    void    Resume(Engine* engine);
    static  Edit_Anim* Instance() {
        return &m_Edit_Anim;
    }

protected:
    Edit_Anim() {}

private:
    static Edit_Anim    m_Edit_Anim;
    Engine*        e;
    Boxes               m_Boxes;

    void        DrawHud();
    void        DrawLine(std::vector<sf::Vector2f>& line);
    void        DrawGrid();
    void        AddToAnimDB(std::string file_name);

    bool        m_recording;
    unsigned    m_grid = GRID_OFF;
    sf::Time    m_recordStart;
    sf::Time    m_recordEnd;
    float       m_interval;
    float       m_prevInterval;
    float       m_time;
    unsigned    m_mode = MODE_NONE;

    std::vector<sf::Vector2f>   m_lineRed;
    std::vector<sf::Vector2f>   m_lineYellow;
    std::vector<sf::Vector2f>   m_lineGreen;
    std::vector<sf::Vector2f>   m_lineBlue;
    std::vector<sf::Vector2f>   m_lineCyan;
    std::vector<sf::Vector2f>   m_lineMagenta;
    std::vector<sf::Vector2f>   m_lineWhite;

    std::vector<sf::Vector2f>*  m_line;
    sf::Vector2f m_lastPos;
    std::string ComposeFileList();
};

#endif
