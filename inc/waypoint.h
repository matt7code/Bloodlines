#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "SFML/Graphics.hpp"
#include <string>

struct Waypoint {
    std::string tag             = "None";
    std::string dialog_trigger  = "NONE";
    float pause                 = 0.0f;
    float speedMod              = 1.0f;
    bool m_grabbed              = false;
    sf::Vector2f pos;
    int m_zLevel;
};

#endif // WAYPOINT_H
