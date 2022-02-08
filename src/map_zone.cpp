#include "map_zone.h"

Zone::Zone() {
    tiles.reserve(ZONE_SIZE_X * ZONE_SIZE_Y);
}

Zone::~Zone() {
}

sf::Vector3i Zone::deriveMapIndexFromWorldPosition(const sf::Vector3f& pos) {
    return sf::Vector3i(pos.x / (ZONE_SIZE_X * TILE_SIZE_X), pos.y / (ZONE_SIZE_Y * TILE_SIZE_Y), pos.z);
}

void Zone::Update(const sf::Clock& clock, const sf::Vector2f& pos, sf::IntRect& viewExtents) {
    //std::cout << tiles.size() << std::endl;
    for(auto& t : tiles) {
        if (t.getSprite().getPosition().x > pos.x - viewExtents.left    &&
                t.getSprite().getPosition().x < pos.x + viewExtents.width   &&
                t.getSprite().getPosition().y > pos.y - viewExtents.top     &&
                t.getSprite().getPosition().y < pos.y + viewExtents.height) {
            t.setWillBeDrawn(true);
            if(t.getIsAnimated()) t.Update(clock);
        } else {
            t.setWillBeDrawn(false);
        }
    }
}

//void Zone::Draw(const sf::Clock& clock, sf::RenderWindow& window, const sf::Vector2f& pos)
void Zone::Draw(sf::RenderWindow& window) {
    for(auto& t : tiles) {
        if(t.getWillBeDrawn()) window.draw(t.getSprite());
    }
    for(auto& o : m_objects) {
        window.draw(o.m_sprite);
    }
}

void Zone::Draw(sf::RenderWindow& window, sf::Shader& shader) {
    for(auto& t : tiles) {
        if(t.getWillBeDrawn()) window.draw(t.getSprite(), &shader);
    }
    for(auto& o : m_objects) {
        window.draw(o.m_sprite, &shader);
    }
}

void Zone::Load(const sf::Vector3f& pos, std::vector<Tile>& tileset, const std::string& name) {
    mapIndex    = deriveMapIndexFromWorldPosition(pos);
    mapName     = BuildName(pos);
    levelName   = name;
    m_objects.clear();

    // Open map file.
    std::ifstream in(levelName + "/zones/" + mapName + ".map");
    if(!in) {
        //std::cout << "Making Zone: " << mapName << std::endl;
        std::ofstream out;
        out.open(levelName + "/zones/" + mapName + ".map");
        if(!out) {
            std::cerr << "Failed to create file: " << levelName + "/zones/" + mapName + ".map" << std::endl;
            return;
        }
        out << "[HEADER]" << std::endl;
        out << levelName + "/zones/" + mapName + ".map" << std::endl;
        out << "[END]" << std::endl;

        out << "[MAPARRAY]" << std::endl;
        std::cout << "[New Zone: " << mapName << ".map]" << std::endl;

        /* Fill in blank field array: */
        /* Ocean */
        if ((int)pos.z == 0) {
            for(int i = 0; i < ZONE_SIZE_Y; ++i) {
                for(int j = 0; j < ZONE_SIZE_X; ++j) out << "7 -1 0.5 8 336 8 337 8 338 8 337 -1 -1 ";
                out << std::endl;
            }
        }
        /* Air */
        else if ((int)pos.z > 0) {
            for(int i = 0; i < ZONE_SIZE_Y; ++i) {
                for(int j = 0; j < ZONE_SIZE_X; ++j) out << "12 2 ";
                out << std::endl;
            }
        }
        /* Underground */
        else if ((int)pos.z < 0) {
            for(int i = 0; i < ZONE_SIZE_Y; ++i) {
                for(int j = 0; j < ZONE_SIZE_X; ++j) out << "8 75 ";
                out << std::endl;
            }
        }
        out << "[END]" << std::endl;
        out.close();

        Load(pos, tileset, levelName);
    } else {
        //std::cout << "Loading Zone: " << mapName << std::endl;
        std::vector<std::string> text_map;
        std::string buffer;

        // Skip comments
        while(std::getline(in, buffer)) {
            if(buffer[0] != '#') text_map.push_back(buffer);
        }

        // Done reading map file.
        in.close();
        for(auto it = text_map.begin(); it != text_map.end(); ++it) {
            if(*it == "[OBJECTS]") {
                int n = 0;
                while(*(++it) != "[END]") {
                    std::stringstream ss;
                    ss.str(*it);

                    Object newObject;

                    int ID;
                    float x, y;
                    int z;
                    float sx, sy;
                    float m_rotation;
                    unsigned int r, g, b, a;

                    ss >> ID >> x >> y >> z >> sx >> sy >> m_rotation >> r >> g >> b >> a;

                    newObject.m_ID = ID;
                    newObject.m_sprite = tileset[ID].getSprite();
                    newObject.m_zLevel = z;
                    newObject.m_sprite.setPosition(sf::Vector2f(x, y));
                    Tool::centerSprite(newObject.m_sprite);
                    newObject.m_sprite.setScale(sf::Vector2f(sx, sy));
                    newObject.m_sprite.setRotation(m_rotation);
                    newObject.m_sprite.setColor(sf::Color(r, g, b, a));

                    m_objects.push_back(newObject);
                    n++;
                }
                std::cout << n << " objects loaded from zone " <<  mapName << std::endl;
            }
            if(*it == "[MAPARRAY]") {
                tiles.clear();
                int line = 0;
                int tile_count = 0;

                while(line != ZONE_SIZE_Y) {
                    std::stringstream ss;
                    ss.str(*(++it));
                    int attr, vis; // Attributes and Visible Tile
                    std::bitset<32> attributes;
                    while(ss >> attr >> vis) {
                        // Convert terrain to bitset:
                        attributes = attr;

                        if (!attributes.test(ATTRIBUTE_ANIMATED)) { // Not animated
                            //createTerrainBitset();
                            tiles.push_back(LoadTile(tile_count, vis, mapIndex.x, mapIndex.y, tileset, attributes));
                        } else {
                            Tile t; // Main Tile
                            float dura;
                            {
                                t = LoadTile(tile_count, 0, mapIndex.x, mapIndex.y, tileset, attributes);
                                // Set the main 'placeholder' tile to animated.
                                // Will already be set in attributes //t.animated = true;
                                // Very next item will be a float meaning the duration between frames.
                                ss >> dura;
                                /// Failsafe for zero duration animations
                                if ( dura == 0 ) dura = 0.1f;
                                ss >> attr >> vis;

                                while (attr != -1) {
                                    attributes = attr;
                                    /// Prevent infinite anim errors:
                                    if(attributes.test(ATTRIBUTE_ANIMATED)) {
                                        std::cerr << "Error loading zone: Nested Animated Tiles." << std::endl;
                                        attributes.set(ATTRIBUTE_ANIMATED, false);
                                    }
                                    t.getAnimTiles().push_back(LoadTile(tile_count, vis, mapIndex.x, mapIndex.y, tileset, attributes));
                                    ss >> attr >> vis;
                                }
                            }
                            t.setDuration(dura);
                            tiles.push_back(t);
                        }
                        ++tile_count;
                    }
                    ++line;
                }
            }
        }
    }
}

void Zone::Save() {
    if (!m_bChanged) return;
    std::ofstream of(levelName + "/zones/" + mapName + ".map");
    if(!of) {
        std::cerr << "Zone::Save() - Fatal Error: Failed to create zonefile for saving." << std::endl;
        return;
    }
    of << "[HEADER]" << std::endl;
    of << levelName + "/zones/" + mapName + ".map" << std::endl;
    of << "[END]" << std::endl;

    of << "[MAPARRAY]";
    int counter = 0;
    //std::cout << "Saving zone";
    for(auto &tile : tiles) {
        if(counter % ZONE_SIZE_X == 0) of << std::endl; // Line breaks

        if (!tile.getIsAnimated()) { // Static Tile
            //std::cout << ".";
            of << tile.getAttributes() << " " << tile.getType() << " ";
        } else { // Animated Tile
            //std::cout << "!" << std::endl;
            of << tile.getAttributes() << " -1 " << tile.getDuration() << " ";
            for (auto& subtile : tile.getAnimTiles()) {
                of << subtile.getAttributes() << " " << subtile.getType() << " ";
            }
            of << "-1 -1 ";
        }
        ++counter;
    }
    //std::cout << counter << " tiles saved." << std::endl;
    of << std::endl << "[END]" << std::endl;
    if(!m_objects.empty()) {
        std::clog << "LOG: Objects Saved" << std::endl;
        of << "[OBJECTS]" << std::endl;
        for(const auto& o : m_objects) {
            of << o.m_ID << "\t" << o.m_sprite.getPosition().x << "\t" << o.m_sprite.getPosition().y << "\t" << o.m_zLevel << "\t"
               << o.m_sprite.getScale().x << "\t" << o.m_sprite.getScale().y << "\t" << o.m_sprite.getRotation() << "\t" << (int)o.m_sprite.getColor().r << "\t"
               << (int)o.m_sprite.getColor().g << "\t" << (int)o.m_sprite.getColor().b << "\t" << (int)o.m_sprite.getColor().a << std::endl;
        }
        of << "[END]" << std::endl;
    }
    of.close();
}

/// @brief Creates and returns a zone tile. The tile is copied from the vector of tiles,
/// and the modified according to the supplied arguments and then returned.
/// @param tileCount    The index of this tile within the zone.
/// @param tileID       The index for this tile's texture.
/// @param map_index    The index of the map.
/// @param tileset      A reference to a vector of tiles this tile is being spawned from.
/// @return The created tile.
Tile Zone::LoadTile(int tileCount, int tileID, int map_index_x, int map_index_y, std::vector<Tile>& tileset, std::bitset<32> _attributes) {
    Tile t;
    if(tileID < 0) std::cout << "TileID less than zero!" << std::endl;
    t = tileset[tileID];
    t.setType(tileID);

    // Set the attributes
    t.setAttributes(_attributes.to_ulong());

    // Get tile's zone index.
    sf::Vector2i zoneIndex  = sf::Vector2i(tileCount % ZONE_SIZE_X, tileCount / ZONE_SIZE_X);

    // Convert zone index to zone coords.
    sf::Vector2f tilePos    = sf::Vector2f(zoneIndex.x * TILE_SIZE_X, zoneIndex.y * TILE_SIZE_Y);

    // Add the offset
    tilePos.x += (map_index_x * (ZONE_SIZE_X * TILE_SIZE_X));
    tilePos.y += (map_index_y * (ZONE_SIZE_Y * TILE_SIZE_Y));

    // Set the position
    t.getSprite().setPosition(tilePos);
    return t;
}

/// @brief Returns the zone lable that would contain the given level position.
/// @param pos          The level position.
/// @return The zone label.
std::string Zone::BuildName(const sf::Vector3f &pos) {
    // Build Label from Coords
    std::stringstream ss;
    ss << (pos.x < 0 ? 'n' : 'p');

    int x = std::abs((int)pos.x / (ZONE_SIZE_X * TILE_SIZE_X));
    if(x < 100) ss << '0';
    if(x < 10) ss << '0';
    ss << x;
    //  mapIndex.x = x;

    ss << (pos.y < 0 ? 'n' : 'p');
    int y = std::abs((int)pos.y / (ZONE_SIZE_Y * TILE_SIZE_Y));
    if(y < 100) ss << '0';
    if(y < 10) ss << '0';
    ss << y;
//   mapIndex.y = y;

    ss << (pos.z < 0 ? 'n' : 'p');
    int z = std::abs((int)pos.z);
    if(z < 100) ss << '0';
    if(z < 10) ss << '0';
    ss << z;
//    mapIndex.z = z;

    return ss.str();
}

/// \brief Determine if a tile at a specified position is passable.
/// \param pos position within the world
/// \return Is the position passable?
bool Zone::isPassable(const sf::Vector2f& pos) const {
    /// @TODO Later, this should supply the current mode of travel, and check IsPassable relative to mode of travel.
//    std::cout << " >>> Zone::isPassable Position: " << pos.x << ", " << pos.y << std::endl;
//    std::cout << " >>> getIndex(pos): " << getIndex(pos) << std::endl;
    //if(getIndex(pos) > tiles.size()) return false;
    return tiles[getIndex(pos)].getIsWalkable();
}

/// \brief Get a tile's numeric texture index at a specified position
/// \param pos position within the world
/// \return zone array index at that position, NOT the tileType
int Zone::getIndex(sf::Vector2f pos) {
    return ((int)pos.x % 1024) / 32 + (((int)pos.y % 640) / 32) * 32;
}

Tile& Zone::getTile(sf::Vector2f pos) {
    return tiles[getIndex(pos)];
}

int Zone::getTileTypeAtPos(sf::Vector2f pos) {
    return getTile(pos).getType();

}

void Zone::setTileTypeAtPos(sf::Vector2f pos, int type) {
    getTile(pos).setType(type);
    getTile(pos).setIsWalkable(true);
}
