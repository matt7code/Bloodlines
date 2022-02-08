#ifndef ZONE_H
#define ZONE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <bitset>
#include <tool.h>
#include "map_tile.h"
#include "object.h"
#include "constants.h"
#include "map_tile_attributes.h"

class Zone {
public:
    Zone();
    virtual ~Zone();

    void                Load(const sf::Vector3f& pos, std::vector<Tile>& tileset, const std::string& name);
    void                Save();
    void                Update(const sf::Clock& clock, const sf::Vector2f& pos, sf::IntRect& viewExtents);
    void                Draw(sf::RenderWindow& window);
    void                Draw(sf::RenderWindow& window, sf::Shader& shader);

    std::vector<Tile>&  getTilesRef() {
        return tiles;
    }
    std::string         getName() const {
        return mapName;
    }
    static std::string  BuildName(const sf::Vector3f& pos);
    Tile                LoadTile(int tileCount, int tileID, int map_index_x, int map_index_y, std::vector<Tile>& tileset, std::bitset<32> _attributes);
    bool                isPassable(const sf::Vector2f& pos) const;
    static int          getIndex(sf::Vector2f pos);
    static sf::Vector3i deriveMapIndexFromWorldPosition(const sf::Vector3f& pos);

    void                setChanged(bool changed) {
        m_bChanged = changed;
    }
    Tile&               getTile(sf::Vector2f pos);
    bool                getChanged() {
        return m_bChanged;
    }
    int                 getTileTypeAtPos(sf::Vector2f pos);
    void                setTileTypeAtPos(sf::Vector2f pos, int type);

    sf::Vector3i        getMapIndex() {
        return mapIndex;
    }
    std::vector<Object> m_objects;

private:
    std::vector<Tile>   tiles; /**< The collection of Tiles drawn every frame. */
    sf::Vector3i        mapIndex;
    std::string         mapName;
    std::string         levelName;
    bool                m_bChanged = false;
};

#endif // ZONE_H
