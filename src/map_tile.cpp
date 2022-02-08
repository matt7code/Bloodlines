#include "map_tile.h"

bool Tile::getIsWalkable() const {
    if(attributes.test(ATTRIBUTE_ANIMATED))
        return AnimTiles[frame].attributes.test(ATTRIBUTE_WALKABLE);
    return attributes.test(ATTRIBUTE_WALKABLE);
}

void Tile::setKey(unsigned short val) {
    std::bitset<8> val_bit(val);
    /// Clear previous value
    attributes.reset(24);
    attributes.reset(25);
    attributes.reset(26);
    attributes.reset(27);
    attributes.reset(28);
    attributes.reset(29);
    attributes.reset(30);
    attributes.reset(31);
    /// Splice into attributes
    if (val_bit.test(0)) attributes.set(24);
    if (val_bit.test(1)) attributes.set(25);
    if (val_bit.test(2)) attributes.set(26);
    if (val_bit.test(3)) attributes.set(27);
    if (val_bit.test(4)) attributes.set(28);
    if (val_bit.test(5)) attributes.set(29);
    if (val_bit.test(6)) attributes.set(30);
    if (val_bit.test(7)) attributes.set(31);
}

unsigned short Tile::getKey() const {
    /// Derive key from attributes
    std::bitset<8> val_bit(0);
    if (attributes.test(24)) val_bit.set(0);
    if (attributes.test(25)) val_bit.set(1);
    if (attributes.test(26)) val_bit.set(2);
    if (attributes.test(27)) val_bit.set(3);
    if (attributes.test(28)) val_bit.set(4);
    if (attributes.test(29)) val_bit.set(5);
    if (attributes.test(30)) val_bit.set(6);
    if (attributes.test(31)) val_bit.set(7);
    return val_bit.to_ulong();
}

void Tile::Update(const sf::Clock& clock) {
    if(AnimTiles.empty()) {
        std::cerr << "Error: Animated Tile has empty animation vector." << std::endl;
        return;
    }
    int ticks_per_frame = duration * ANIMATED_TILE_TICKS_PER_SECOND;
    int actual_tick = clock.getElapsedTime().asSeconds() * ANIMATED_TILE_TICKS_PER_SECOND;
    int c = actual_tick % (AnimTiles.size() * ticks_per_frame);
    frame = c / ticks_per_frame;
    sprite = AnimTiles[frame].sprite;
}

//void Tile::updateSprite()
//{
//    sprite = AnimTiles[frame].sprite;
//    sprite = getSprite().setTextureRect(sf::IntRect(x * TILE_SIZE_X, y * TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y));
//}

void Tile::setType(unsigned int type) {
    //std::cout << "TEST" << std::endl;
    tileType = type;
    sprite.setTextureRect(sf::IntRect((type%24) * TILE_SIZE_X, (type/24) * TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y));
}
