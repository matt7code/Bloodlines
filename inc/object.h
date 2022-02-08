#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>

class Object {
public:
    Object() {};
    virtual ~Object() {};

    int             m_ID;
    int             m_zLevel;
    sf::Sprite      m_sprite;
};

#endif // OBJECT_H
