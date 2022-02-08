#ifndef NPC_H
#define NPC_H

#include <vector>
#include <string>
#include "constants.h"
#include "waypoint.h"
#include "tool.h"
#include <SFML/Graphics.hpp>

class Npc {
public:
    bool            m_active;
    enum            { MOVE_STATIONARY, MOVE_PATROL_CIRCLE, MOVE_PATROL_BACK_AND_FORTH, MOVE_MARCHING_IN_PLACE, MOVE_FOLLOW, MOVE_RANDOM };
    Npc();
    virtual ~Npc()  {}

    void            Update(float delta_seconds);
    void            Draw(sf::RenderWindow& rw);
    void            UpdateAppearance();

    int             ID;
    sf::Color       color;
    int             value1  = 0;
    int             value2  = 0;
    int             value3  = 0;
    std::string     name    = "Unknown NPC";
    std::string     tag     = "NONE";

    int             moveMode= MOVE_STATIONARY;
    int             avatar  = 0;
    std::vector<Waypoint>
    waypoints;

    sf::Vector2i    textureSheetBase;
    float           speed = NPC_SPEED_DEFAULT;
    sf::Vector2f    OriginalPosition;

    sf::Sprite      sprite;
    int             m_zLevel = 0;

    float           animTimer       = 0.0f;
    int             animFrame       = 0;
    int             animFrameSet[4] = {0, 1, 2, 1};
    int             facing  = DIR_SOUTH;

    /// Waypoint Patrol
    int             m_prevWP = 0;
    int             m_currWP = 0;
    float           m_distance = 0.0f;
    sf::Vector2f    m_direction;
    sf::Vector2f    m_startPos;

    bool            m_grabbed = false;
    bool            m_moving_forward_on_path = true;
};

#endif // NPC_H
