#include "map_level.h"

/// @brief Load a level by name
/// @param _name    The name of the level.
void Level::Load(const std::string& level_name) {
    name = level_name;
    if (name != "book1") {
        std::cerr << "Level name not found: " << name << std::endl;
        return;
    }

    std::cout << "Loading: Level Graphics" << std::endl;
    LoadGraphics(name);
    std::cout << "Loading: Sounds" << std::endl;
    LoadSounds(name);
    std::cout << "Loading: Enemy Roster" << std::endl;
    LoadEnemyRoster(name);
    std::cout << "Loading: Beacons" << std::endl;
    LoadBeacons(name);
    std::cout << "Loading: Waypoints" << std::endl;
    LoadWaypoints(name);
    std::cout << "Loading: NPCs" << std::endl;
    LoadNpcs(name);
    std::cout << "Loading: Dialogs" << std::endl;
    LoadDialogs(name);
    std::cout << "Loading: Triggers" << std::endl;
    LoadTriggers(name);
    std::cout << "Loading: Faces" << std::endl;
    PopulateFaceVector();
    std::cout << "Loading: Terrain" << std::endl;
    PopulateTerrainSpriteVector();
}

std::vector<std::string>& Level::getDialog(std::string tag) {
    auto it = m_Dialogs.find(tag);
    if (it != m_Dialogs.end()) return it->second;
    else return m_Dialogs["NOTFOUND"];
}


bool Level::LoadDialogs(std::string level) {
    std::vector<std::string> convo;
    std::string input;
    std::fstream in(level + "/dialogs");
    if (!in) {
        std::cerr << "Error loading " << level << "/dialogs" << std::endl;
        return false;
    } else {
        std::stringstream ss;
        std::string currTag = "NONE";
        //std::vector<std::string> TokenizedLine;
        while(std::getline(in, input)) {
            if(input[0] == '#') continue;
            if(input.empty()) continue;
            ss.clear();
            ss.str(input);
            std::string word;
            ss >> word;

            if(word == "NPC") {
                ss >> currTag;
            } else {
                m_Dialogs[currTag].push_back(input);
            }
        }
        //DebugDialogTree();
    }

    return true;
}

bool Level::SaveTriggers(std::string level) {
    std::ofstream of(level + "/triggers");
    bool ret = false;
    if (!of) {
        std::cerr << "Error opening " << level << "/triggers for saving." << std::endl;
        ret = false;
    } else {
        of << "#  Tag >> Area.left >> Area.top >> Area.width >> Area.height >> ZLevel" << std::endl;

        for(auto t : m_Triggers) {
            of << t.Tag << "\t" << t.Area.left  << "\t" << t.Area.top << "\t"
               << t.Area.width << "\t" << t.Area.height << "\t" << t.ZLevel << "\t" << std::endl;
        }
        of.close();
        ret = true;
    }
    return ret;
}

bool Level::LoadTriggers(std::string level) {
    std::string input;
    std::fstream in(level + "/triggers");
    if (!in) {
        std::cerr << "Error loading " << level << "/triggers" << std::endl;
        return false;
    } else {
        std::stringstream ss;
        while(std::getline(in, input)) {
            if(input[0] == '#') continue;
            if(input.empty()) continue;
            ss.clear();
            ss.str(input);

            Trigger t;
            ss >> t.Tag >> t.Area.left >> t.Area.top >> t.Area.width >> t.Area.height >> t.ZLevel;
            m_Triggers.push_back(t);
        }
    }
    in.close();
    std::clog << "Number of Triggers loaded: " << m_Triggers.size() << std::endl;
//    for(auto t : m_Triggers) {
//        std::cout << "Trigger Tag: " << t.Tag << "\tLocation: " << t.Area.left << "x, " << t.Area.top << "y, " << t.ZLevel << "z" << "\tSize: " << t.Area.width << " by " << t.Area.height << std::endl;
//    }
    return true;
}

void Level::DebugDialogTree() {
    for (auto it = m_Dialogs.begin(); it != m_Dialogs.end(); ++it) {
        std::cout << "Dialog found: " << it->first << " Size: " << it->second.size() << " lines." << std::endl;
    }
}

/// @brief Update the level.
/// @param clock    A reference to the World Clock.
/// @param pos      The world position.
void Level::Update(const sf::Clock& clock, const sf::Vector2f& pos, sf::IntRect& viewExtents) {
    sf::Vector3i currLocation = sf::Vector3i(pos.x, pos.y, z_Level);
    if (currLocation != prevLocation) {
        prevLocation = currLocation;

        // Build list of needed new zones:
        nz1 = Zone::BuildName(sf::Vector3f(pos.x - (ZONE_SIZE_X * TILE_SIZE_X / 2), pos.y - (ZONE_SIZE_Y * TILE_SIZE_Y / 2), z_Level));
        nz2 = Zone::BuildName(sf::Vector3f(pos.x - (ZONE_SIZE_X * TILE_SIZE_X / 2), pos.y + (ZONE_SIZE_Y * TILE_SIZE_Y / 2), z_Level));
        nz3 = Zone::BuildName(sf::Vector3f(pos.x + (ZONE_SIZE_X * TILE_SIZE_X / 2), pos.y - (ZONE_SIZE_Y * TILE_SIZE_Y / 2), z_Level));
        nz4 = Zone::BuildName(sf::Vector3f(pos.x + (ZONE_SIZE_X * TILE_SIZE_X / 2), pos.y + (ZONE_SIZE_Y * TILE_SIZE_Y / 2), z_Level));
        newZoneList = nz1 + ' ' + nz2 + ' ' + nz3 + ' ' + nz4;
        oldZoneList.clear();

        for (auto it = area.begin(); it != area.end();) {
            if (newZoneList.find(it->getName()) == std::string::npos) {
                it->Save();
                area.erase(it);
            } else {
                oldZoneList += it->getName() += ' ';
                ++it;
            }
        }

        if (area.size() != 4) {
            Zone z;
            if (std::string::npos == oldZoneList.find(nz1)) {
                z.Load(sf::Vector3f(pos.x - (ZONE_SIZE_X * TILE_SIZE_X / 2), pos.y - (ZONE_SIZE_Y * TILE_SIZE_Y / 2), z_Level), tileset, name);
                area.push_back(z);
            }
            if (std::string::npos == oldZoneList.find(nz2)) {
                z.Load(sf::Vector3f(pos.x - (ZONE_SIZE_X * TILE_SIZE_X / 2), pos.y + (ZONE_SIZE_Y * TILE_SIZE_Y / 2), z_Level), tileset, name);
                area.push_back(z);
            }
            if (std::string::npos == oldZoneList.find(nz3)) {
                z.Load(sf::Vector3f(pos.x + (ZONE_SIZE_X * TILE_SIZE_X / 2), pos.y - (ZONE_SIZE_Y * TILE_SIZE_Y / 2), z_Level), tileset, name);
                area.push_back(z);
            }
            if (std::string::npos == oldZoneList.find(nz4)) {
                z.Load(sf::Vector3f(pos.x + (ZONE_SIZE_X * TILE_SIZE_X / 2), pos.y + (ZONE_SIZE_Y * TILE_SIZE_Y / 2), z_Level), tileset, name);
                area.push_back(z);
            }
        }
    }
    for (auto& z : area) z.Update(clock, pos, viewExtents);

//    for(auto& npc : e->m_Level.Npcs) {
//        if (Tool::dist(npc.sprite.getPosition(), e->m_View.getCenter()) < CLIP_DISTANCE_NPCS) npc.Update(delta);
//    }

    //if (m_NPCdelta > 0.0333333f)

    m_NPCdelta = clock.getElapsedTime().asSeconds() - m_prevNPCUpdateTime.asSeconds();
    if(m_NPCdelta > 0.5f) m_NPCdelta = 0.5f; // Catch for long pauses
    for(auto& npc : Npcs) {
        if(npc.m_zLevel == z_Level && Tool::dist(npc.sprite.getPosition(), pos) < CLIP_DISTANCE_NPCS) npc.Update(m_NPCdelta);
    }
    m_prevNPCUpdateTime = clock.getElapsedTime();
}

/// @brief Draw the level & objects by zone. Npcs separately.
/// @param window   A reference to the sf::RenderWindow to draw too.
void Level::Draw(sf::RenderWindow& window, bool editing) {
    for(auto& zone : area) zone.Draw(window);

    for(auto& n : Npcs) {
        if(n.m_zLevel == z_Level && (Tool::dist(n.sprite.getPosition(), window.getView().getCenter()) < CLIP_DISTANCE_NPCS) && (editing || n.avatar != 41)) window.draw(n.sprite);
    }
}

void Level::Draw(sf::RenderWindow& window, sf::Shader& shader, bool editing)
{
    for(auto& zone : area) zone.Draw(window, shader);

    for(auto& n : Npcs) {
        if(n.m_zLevel == z_Level && (Tool::dist(n.sprite.getPosition(), window.getView().getCenter()) < CLIP_DISTANCE_NPCS) && (editing || n.avatar != 41)) window.draw(n.sprite);
    }
}

/// \brief Return a reference to the currently occupied Zone
/// \param pos          Position of Party
/// \return Zone&           A Zone reference.
/// \see BuildName
Zone& Level::getZone(sf::Vector2f pos) {
    std::string targetZone = Zone::BuildName(sf::Vector3f(pos.x, pos.y, z_Level));
    for (auto& z : area) if (z.getName() == targetZone) return z;
    std::cerr << "Warning: Level::getZone failure!" << std::endl;
    return area[0];
}

sf::Vector2f Level::getAreaOrigin() {
    sf::Vector3i origin = area.begin()->getMapIndex();

    for(auto& z : area) {
        if (z.getMapIndex().x < origin.x || z.getMapIndex().y < origin.y) origin = z.getMapIndex();
    }
    return sf::Vector2f(origin.x * TILE_SIZE_X * ZONE_SIZE_X, origin.y * TILE_SIZE_Y * ZONE_SIZE_Y);
}

bool Level::IsFading() {
    return m_FadeAlpha != 0;
}

void Level::PlayStairs(float pitch) {
    m_Sound_Stairs.setPitch(pitch);
    m_Sound_Stairs.play();
}

void Level::PlaySleep(float pitch) {
    m_Sound_Sleep.setPitch(pitch);
    m_Sound_Sleep.play();
}

void Level::PlayDoor(float pitch) {
    m_Sound_Door.setPitch(pitch);
    m_Sound_Door.play();
}

void Level::PlayJiggle(float pitch) {
    if(m_Sound_DoorJiggle.getStatus() != sf::Sound::Playing) {
        m_Sound_DoorJiggle.setPitch(pitch - 0.5f);
        m_Sound_DoorJiggle.play();
    }
}


Beacon* Level::getNearestCombatBeacon(sf::Vector3i pos) {
    Beacon* ret = nullptr;

    float olddist = 1000000.0f; // ~30 Zones

    for (auto& b : m_CombatBeacons) {
        if(b.getPosition().z != pos.z) continue;

        float newdist = Tool::dist(sf::Vector2f(pos.x, pos.y), sf::Vector2f(b.getPosition().x, b.getPosition().y));

        if(newdist > b.getValue1()) continue;

        if (newdist < olddist) {
            olddist = newdist;
            ret = &b;
        }
    }
    return ret;
}

Beacon* Level::getNearestSearchBeacon(sf::Vector3i pos) {
    Beacon* ret = nullptr;

    float olddist = 1000.0f; // ~1 squares (Search Power?)

    for (auto& b : m_SearchBeacons) {
        if(b.getPosition().z != pos.z) continue;

        float newdist = Tool::dist(sf::Vector2f(pos.x, pos.y), sf::Vector2f(b.getPosition().x, b.getPosition().y));

        if(newdist > b.getValue1()) continue;

        if (newdist < olddist) {
            ret = &b;
            olddist = newdist;
        }
    }
    return ret;
}

Beacon* Level::getNearestInactiveBeacon(sf::Vector3i pos) {
    Beacon* ret = nullptr;

    float olddist = 1000000.0f; // ~10 Zones

    for (auto& b : m_InactiveBeacons) {
        if(b.getPosition().z != pos.z) continue;

        float newdist = Tool::dist(sf::Vector2f(pos.x, pos.y), sf::Vector2f(b.getPosition().x, b.getPosition().y));

        if(newdist > b.getValue1()) continue;

        if (newdist < olddist) {
            ret = &b;
            olddist = newdist;
        }
    }
    return ret;
}

Beacon* Level::getNearestLocationBeacon(sf::Vector3i pos) {
    Beacon* ret = nullptr;

    float olddist = 1000000.0f; // ~10 Zones

    for (auto& b : m_LocationBeacons) {
        if(b.getPosition().z != pos.z) continue;

        float newdist = Tool::dist(sf::Vector2f(pos.x, pos.y), sf::Vector2f(b.getPosition().x, b.getPosition().y));

        if(newdist > b.getValue1()) continue;

        if (newdist < olddist) {
            ret = &b;
            olddist = newdist;
        }
    }
    return ret;
}

void Level::LoadWaypoints(std::string level_name) {
    WaypointMap.clear();

    std::ifstream in(level_name + "/waypoints");
    std::string input;

    while (std::getline(in, input)) {
        if(input[0] == '#') continue;
        std::stringstream ss;
        ss.str(input);
        Waypoint wp;
        ss >> wp.tag >> wp.dialog_trigger >> wp.pause >> wp.speedMod >> wp.pos.x >> wp.pos.y >> wp.m_zLevel;
        WaypointMap[wp.tag].push_back(wp);
    }
    in.close();
}

void Level::LoadNpcs(std::string level_name) {
    Npcs.clear();

    std::ifstream in(level_name + "/npcs");
    std::string input;

    while (std::getline(in, input)) {
        if(input[0] == '#' || input.empty()) continue;
        std::stringstream ss;
        Npc newNpc;
        newNpc.sprite.setTexture(npc_texture);
        newNpc.name = input;
        std::getline(in, input);
        newNpc.tag = input;

        std::getline(in, input);
        ss.str(input);
        float temp_x, temp_y;
        int color_red, color_green, color_blue, color_alpha;
        ss >> newNpc.ID >> newNpc.value1 >> newNpc.value2 >> newNpc.value3 >> newNpc.moveMode;
        ss >> newNpc.facing >> newNpc.avatar >> temp_x >> temp_y >> newNpc.m_zLevel;
        ss >> color_red >> color_green >> color_blue >> color_alpha;

        newNpc.color.r = color_red;
        newNpc.color.g = color_green;
        newNpc.color.b = color_blue;
        newNpc.color.a = color_alpha;
        newNpc.sprite.setColor(newNpc.color);

        /// Waypoints
        for(auto it = WaypointMap.begin(); it != WaypointMap.end(); ++it) {
            if(it->first == newNpc.tag) {
                newNpc.waypoints = it->second;
                temp_x = newNpc.waypoints.begin()->pos.x;
                temp_y = newNpc.waypoints.begin()->pos.y;
            }
        }
        newNpc.sprite.setPosition(temp_x, temp_y);
        newNpc.OriginalPosition.x = temp_x;
        newNpc.OriginalPosition.y = temp_y;

        if(DEBUG_LOAD_NPC) {
            std::cout << std::endl;
            std::cout << "NPC Loaded!" << std::endl;
            std::cout << "Name:   " << newNpc.name << std::endl;
            std::cout << "Tag:    " << newNpc.tag << std::endl;
            std::cout << "ID:     " << newNpc.ID << std::endl;
            std::cout << "Color:  " << newNpc.color.r << " " << newNpc.color.g << " " << newNpc.color.b << " " << newNpc.color.a << std::endl;
            std::cout << "Value1: " << newNpc.value1 << std::endl;
            std::cout << "Value2: " << newNpc.value2 << std::endl;
            std::cout << "Value3: " << newNpc.value3 << std::endl;
            std::cout << "mvMode: " << newNpc.moveMode << std::endl;
            std::cout << "Facing: " << newNpc.facing << std::endl;
            std::cout << "Avatar: " << newNpc.avatar << std::endl;
            std::cout << "Pos.x : " << newNpc.sprite.getPosition().x << std::endl;
            std::cout << "Pos.y : " << newNpc.sprite.getPosition().y << std::endl;
            std::cout << "zLevel: " << newNpc.m_zLevel << std::endl;
        }
        newNpc.UpdateAppearance();
        Npcs.push_back(newNpc);
    }
    in.close();
}

void Level::LoadBeacons(std::string level_name) {
    m_InactiveBeacons.clear();
    m_CombatBeacons.clear();
    m_SearchBeacons.clear();
    m_LocationBeacons.clear();

    // Combat Becons
    std::ifstream in(level_name + "/beacons");
    std::string input;

    while (std::getline(in, input)) {
        int type, posX, posY, posZ;
        float v1, v2, v3, v4, v5;
        std::string t1, t2;

        std::stringstream ss;
        ss << input;
        ss >> type >> posX >> posY >> posZ >> v1 >> v2 >> v3 >> v4 >> v5;
        std::getline(in, t1);
        std::getline(in, t2);

        Beacon beacon;
        beacon.setType(type);
        beacon.setPosition(sf::Vector3i(posX, posY, posZ));
        beacon.setValue1(v1);
        beacon.setValue2(v2);
        beacon.setValue3(v3);
        beacon.setValue4(v4);
        beacon.setValue5(v5);
        beacon.setText1(t1);
        beacon.setText2(t2);
        beacon.setText1(t1);
        beacon.setText2(t2);

        if(type == Beacon::NONE) {
            LOG("Error: Beacon Loaded with NONE Type");
        }
        if(type == Beacon::INACTIVE) {
            m_InactiveBeacons.push_back(beacon);
        }
        if(type == Beacon::COMBAT) {
            m_CombatBeacons.push_back(beacon);
        }
        if(type == Beacon::LOCATION) {
            m_LocationBeacons.push_back(beacon);
        }
        if(type == Beacon::SEARCH) {
            m_SearchBeacons.push_back(beacon);
        }
    }
    in.close();
}

std::string Level::getPlace(sf::Vector2f pos) {
    Beacon *ret;
    ret = getNearestLocationBeacon(sf::Vector3i(pos.x, pos.y, z_Level));
    if(!ret) return "Unknown";
    return ret->getText1();
}

std::string Level::getCountry(sf::Vector2f pos) {
    Beacon *ret;
    ret = getNearestLocationBeacon(sf::Vector3i(pos.x, pos.y, z_Level));
    if(!ret) return "Unknown";
    return ret->getText2();
}

void Level::SaveBeacons(std::string level_name) {
    std::ofstream out(level_name + "/beacons");

    // Combat Becons
    for(auto& b : m_CombatBeacons) {
        if(b.getType() == Beacon::NONE) continue;
        out << b.getType() << "\t" << b.getPosition().x << "\t" << b.getPosition().y << "\t" << b.getPosition().z << "\t"
            << b.getValue1() << "\t" << b.getValue2() << "\t" << b.getValue3() << "\t" << b.getValue4() << "\t" << b.getValue5() << std::endl;
        out << b.getText1() << std::endl;
        out << b.getText2() << std::endl;
    }
    for(auto& b : m_SearchBeacons) {
        if(b.getType() == Beacon::NONE) continue;
        out << b.getType() << "\t" << b.getPosition().x << "\t" << b.getPosition().y << "\t" << b.getPosition().z << "\t"
            << b.getValue1() << "\t" << b.getValue2() << "\t" << b.getValue3() << "\t" << b.getValue4() << "\t" << b.getValue5() << std::endl;
        out << b.getText1() << std::endl;
        out << b.getText2() << std::endl;
    }
    for(auto& b : m_LocationBeacons) {
        if(b.getType() == Beacon::NONE) continue;
        out << b.getType() << "\t" << b.getPosition().x << "\t" << b.getPosition().y << "\t" << b.getPosition().z << "\t"
            << b.getValue1() << "\t" << b.getValue2() << "\t" << b.getValue3() << "\t" << b.getValue4() << "\t" << b.getValue5() << std::endl;
        out << b.getText1() << std::endl;
        out << b.getText2() << std::endl;
    }
    for(auto& b : m_InactiveBeacons) {
        if(b.getType() == Beacon::NONE) continue;
        out << b.getType() << "\t" << b.getPosition().x << "\t" << b.getPosition().y << "\t" << b.getPosition().z << "\t"
            << b.getValue1() << "\t" << b.getValue2() << "\t" << b.getValue3() << "\t" << b.getValue4() << "\t" << b.getValue5() << std::endl;
        out << b.getText1() << std::endl;
        out << b.getText2() << std::endl;
    }
    out.close();
}

void Level::SaveNpcs(std::string level_name) {
    std::ofstream out(level_name + "/npcs");

    for(auto& n : Npcs) {
        out << n.name << std::endl;
        out << n.tag << std::endl;
        out << n.ID << "\t" << n.value1 << "\t" << n.value2 << "\t" << n.value3 << "\t" << n.moveMode;
        out << "\t" << n.facing << "\t" << n.avatar << "\t" << n.OriginalPosition.x << "\t" << n.OriginalPosition.y << "\t" << n.m_zLevel;
        out << "\t" << (int)n.color.r << "\t" << (int)n.color.g << "\t" << (int)n.color.b << "\t" << (int)n.color.a << std::endl;
    }
    out.close();
}

void Level::SaveWaypoints(std::string level_name) {
    std::ofstream out(level_name + "/waypoints");

    for(auto it = WaypointMap.begin(); it != WaypointMap.end(); ++it) {
        for(auto& wp : it->second) {
            out << Tool::padLeft(wp.tag, 20) << "\t" << Tool::padLeft(wp.dialog_trigger, 20) << "\t" << wp.pause << "\t" << wp.speedMod << "\t";
            out << wp.pos.x << "\t" << wp.pos.y << "\t" << wp.m_zLevel << std::endl;
        }
    }
    out.close();
}

bool Level::LoadSounds(std::string level) {
    if(!m_Sound_StairBuffer.loadFromFile("sounds/stairs.wav")) {
        std::cerr << "Failed to load: sounds/stairs.wav" << std::endl;
        return false;
    }
    m_Sound_Stairs.setBuffer(m_Sound_StairBuffer);

    if(!m_Sound_DoorBuffer.loadFromFile("sounds/door.wav")) {
        std::cerr << "Failed to load: sounds/door.wav" << std::endl;
        return false;
    }
    m_Sound_Door.setBuffer(m_Sound_DoorBuffer);

    if(!m_Sound_SleepBuffer.loadFromFile("sounds/lullaby.wav")) {
        std::cerr << "Failed to load: sounds/lullaby.wav" << std::endl;
        return false;
    }
    m_Sound_Sleep.setBuffer(m_Sound_SleepBuffer);

    if(!m_Sound_DoorJiggleBuffer.loadFromFile("sounds/DoorJiggle.ogg")) {
        std::cerr << "Failed to load: sounds/DoorJiggle.ogg" << std::endl;
        return false;
    }
    m_Sound_DoorJiggle.setBuffer(m_Sound_DoorJiggleBuffer);

    return true;
}

bool Level::LoadEnemyRoster(std::string level) {
    std::ifstream in(level + "/enemies");
    if(!in) {
        std::cerr << "Failed to open enemies file: " << level + "/enemies" << std::endl;
        return false;
    }
    std::string input;
    while(std::getline(in, input)) {
        Enemy baddie;
        if(input[0] == '#') continue;

        std::vector<std::string> enemy_stats = Tool::Parse(input);

        std::stringstream ss(enemy_stats[0]);
        std::cout << "enemy_stats[0]: " << enemy_stats[0] << std::endl;

        std::string word;
        ss >> baddie.m_tag;
        baddie.m_name.clear();
        while (ss >> word) {
            baddie.m_name += word;
            baddie.m_name += ' ';
        }
        baddie.m_name.pop_back();

        ss.clear();
        ss.str(enemy_stats[1]);

        int colorR, colorG, colorB, colorA;
        float scaleX, scaleY;
        ss >> baddie.m_id >> baddie.m_avatar >> baddie.m_level >> baddie.m_bonusHp >> baddie.m_bonusXp >> baddie.m_bonusGold >> colorR >> colorG >> colorB >> colorA >> scaleX >> scaleY;

        baddie.m_color = sf::Color(colorR, colorG, colorB, colorA);
        baddie.m_scale = sf::Vector2f(scaleX, scaleY);

        ss.clear();
        ss.str(enemy_stats[2]);
        ss >> baddie.m_bonusAPhys >> baddie.m_bonusAMagi >> baddie.m_bonusAHeat >> baddie.m_bonusACold >> baddie.m_bonusAElec >> baddie.m_bonusAAcid
           >> baddie.m_bonusDPhys >> baddie.m_bonusDMagi >> baddie.m_bonusDHeat >> baddie.m_bonusDCold >> baddie.m_bonusDElec >> baddie.m_bonusDAcid;


        if(enemy_stats.size() > 3) {
            ss.clear();
            ss.str(enemy_stats[3]);
            int ability;
            while (ss >> ability) baddie.m_abilitiesOffensive.push_back(ability);
        }

        if(enemy_stats.size() > 4) {
            ss.clear();
            ss.str(enemy_stats[4]);
            int ability;
            while (ss >> ability) baddie.m_abilitiesDefensive.push_back(ability);
        }

        DebugEnemyLoad(baddie);
        m_EnemyRoster.push_back(baddie);
    }
    std::cout << "Loaded " << m_EnemyRoster.size() << " enemies." << std::endl;
    return true;
}

void Level::DebugEnemyLoad(Enemy& enemy) {
    std::cout << std::endl;
    std::cout << "=[ " << enemy.m_name << " ]=================================================" << std::endl;
    std::cout << "ID: " << enemy.m_id << std::endl;
    std::cout << "Tag: " << enemy.m_tag << std::endl;
    std::cout << "Name: " << enemy.m_name << std::endl;
    std::cout << "Level: " << enemy.m_level << std::endl;
    std::cout << std::endl;
    std::cout << "AvatarID: " << enemy.m_avatar << std::endl;
    std::cout << "Color: R" << (int)enemy.m_color.r << " G" << (int)enemy.m_color.g << " B" << (int)enemy.m_color.b << " A" << (int)enemy.m_color.a << std::endl;
    std::cout << "Sprite Scale: " << enemy.m_scale.x << "/" << enemy.m_scale.y << std::endl;
    std::cout << std::endl;
//    std::cout << "Bonus Attack: " << enemy.mgetBonusAttack() << std::endl;
//    std::cout << "Bonus Defensive: " << enemy.getBonusDefence() << std::endl;
    std::cout << "Bonus Hit Points: " << enemy.m_bonusHp << std::endl;
    std::cout << "Bonus Experience: " << enemy.m_bonusXp << std::endl;
    std::cout << "Bonus Gold: " << enemy.m_bonusGold << std::endl;
    std::cout << std::endl;
    std::cout << "Offensive Spells: ";
    for(auto i : enemy.m_abilitiesOffensive) std::cout << i << " ";
    std::cout << std::endl;
    std::cout << "Defensive Spells: ";
    for(auto i : enemy.m_abilitiesDefensive) std::cout << i << " ";
    std::cout << std::endl;
}

bool Level::LoadGraphics(std::string level) {
    if(!texture.loadFromFile("graphics/terrain_basic.png")) {
        std::cerr << "Failed to load: graphics/terrain_basic.png" << std::endl;
        return false;
    }
    if(!npc_texture.loadFromFile("graphics/characters.png")) {
        std::cerr << "Failed to load: graphics/characters.png" << std::endl;
        return false;
    }
    if(!m_faces.loadFromFile("graphics/faceset.png")) {
        std::cerr << "Failed to load: graphics/faceset.png" << std::endl;
        return false;
    }
    if(!m_EnemyAvatarTexture.loadFromFile("graphics/book1_monsters.png")) {
        std::cerr << "Failed to load: graphics/book1_monsters.png" << std::endl;
        return false;
    }

    sf::Sprite enemy_sprite;
    enemy_sprite.setTexture(m_EnemyAvatarTexture);


    for(int i = 0; i < 12; ++i) {
        /// Small Enemies (128x128) (12)
        enemy_sprite.setTextureRect(sf::IntRect((i % 4) * 128, (i / 4) * 128, 128, 128));
        m_EnemyAvatars.push_back(enemy_sprite);
    }

    for(int i = 0; i < 8; ++i) {
        /// Medium Enemies (128x256) (8)
        enemy_sprite.setTextureRect(sf::IntRect((i % 4) * 128, (i / 4) * 256 + 384, 128, 256));
        m_EnemyAvatars.push_back(enemy_sprite);
    }

    for(int i = 0; i < 2; ++i) {
        /// Large Enemies (256x256) (2)
        enemy_sprite.setTextureRect(sf::IntRect((i % 2) * 256, (i / 2) * 256 + 896, 256, 256));
        m_EnemyAvatars.push_back(enemy_sprite);
    }

    std::cout << "Enemy Avatars Loaded: " << m_EnemyAvatars.size() << std::endl;
    return true;
}

void Level::PopulateFaceVector() {
    /// Populate the face vector
    sf::Sprite face;
    face.setTexture(m_faces);
    for(int y = 0; y < 4; ++y) {
        for(int x = 0; x < 8; ++x) {
            face.setTextureRect(sf::IntRect(x * 96, y * 96, 96, 96));
            face.setPosition(280, FACE_TOP);
            face.setScale(2.0f, 2.0f);
            m_faceset.push_back(face);
        }
    }
    m_faceset.resize(27); // Remove blank faces
}

void Level::PopulateTerrainSpriteVector() {
    /// Populate the terrain sprite vector
    Tile t;
    t.getSprite().setTexture(texture);
    for(int y = 0; y < 42; ++y) {
        for(int x = 0; x < 24; ++x) {
            t.getSprite().setTextureRect(sf::IntRect(x * TILE_SIZE_X, y * TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y));
            tileset.push_back(t);
        }
    }
}
