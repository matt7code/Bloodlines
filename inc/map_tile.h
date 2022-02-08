#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <bitset>
#include "constants.h"
#include "map_tile_attributes.h"

class Tile {
public:
                            Tile() {};
    virtual                 ~Tile() {};

    void                    Update(const sf::Clock& clock);

    std::vector<Tile>&      getAnimTiles() { return AnimTiles; }
    long                    getAttributes() const { return attributes.to_ulong(); }
    sf::Sprite&             getSprite() { return sprite; }
    float                   getDuration() const { return duration; }
    int                     getType() const { return tileType; }
    unsigned short          getKey() const;
    bool                    getIsLocked() const { return attributes.test(ATTRIBUTE_DOOR_LOCKED); }
    bool                    getIsAnimated() const { return attributes.test(ATTRIBUTE_ANIMATED); }
    bool                    getIsWalkable() const;
    bool                    getWillBeDrawn() const { return attributes.test(ATTRIBUTE_WILL_BE_DRAWN); }

    void                    setAttributes(long int attr) { attributes = attr; }
    void                    setDuration(float _dura) { duration = _dura; }
    void                    setType(unsigned int _tileType);
    void                    setIsWalkable(bool _walkable) { attributes.set(ATTRIBUTE_WALKABLE, _walkable); }
    void                    setIsAnimated(bool _animated) { attributes.set(ATTRIBUTE_ANIMATED, _animated); }
    void                    setIsLocked(bool val) { attributes.set(ATTRIBUTE_DOOR_LOCKED, val); }
    void                    setWillBeDrawn(bool _draw) { attributes.set(ATTRIBUTE_WILL_BE_DRAWN, _draw); }
    void                    setKey(unsigned short val);

private:
    std::vector<Tile>       AnimTiles;
    std::bitset<32>         attributes = 13;
    sf::Sprite              sprite;
    float                   duration = 1.0f;
    int                     tileType = 0;
    int                     frame = 0;
};

#endif // TILE_H
