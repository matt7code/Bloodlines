#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include "tool.h"
#include "beacon.h"
#include "constants.h"
#include "map_zone.h"

#include "npc.h"
#include "waypoint.h"
#include "enemy.h"
#include "trigger.h"

/// @mainpage Zones
/// @par A collection of Zones, e.g. a level map.
class Level {
public:
    Level() {};
    virtual             ~Level() {};

    void                Load(const std::string& level_name);
    void                Update(const sf::Clock& clock, const sf::Vector2f& pos, sf::IntRect& viewExtents);
    void                Draw(sf::RenderWindow& window, bool editing = false);
    void                Draw(sf::RenderWindow& window, sf::Shader& shader, bool editing = false);

    Zone&               getZone(sf::Vector2f pos);
    std::string         getName() {
        return name;
    }
    std::vector<Zone>   area;
    std::vector<Tile>   tileset; /**< A collection of tiles that zones reference to build themselves. */
    int                 z_Level = 0;
    sf::Texture         texture;
    sf::Texture         npc_texture;
    sf::Vector2f        getAreaOrigin();
    bool                IsFading();

    void                PlayStairs(float pitch = 1.0f);
    void                PlaySleep(float pitch = 1.0f);
    void                PlayDoor(float pitch = 1.0f);
    void                PlayJiggle(float pitch = 1.0f);

    std::string         getPlace(sf::Vector2f pos);
    Beacon*             getNearestInactiveBeacon(sf::Vector3i pos);
    Beacon*             getNearestCombatBeacon(sf::Vector3i pos);
    Beacon*             getNearestSearchBeacon(sf::Vector3i pos);
    Beacon*             getNearestLocationBeacon(sf::Vector3i pos);

    std::string         getCountry(sf::Vector2f pos);

    std::vector<Beacon>&    getInactiveBeacons() {
        return m_InactiveBeacons;
    }
    std::vector<Beacon>&    getCombatBeacons() {
        return m_CombatBeacons;
    }
    std::vector<Beacon>&    getSearchBeacons() {
        return m_SearchBeacons;
    }
    std::vector<Beacon>&    getLocationBeacons() {
        return m_LocationBeacons;
    }

    std::vector<Npc>    Npcs;

    std::map<std::string, std::vector<Waypoint>>
            WaypointMap;
    void                resetTimerNPC(const sf::Clock& clock) {
        m_prevNPCUpdateTime = clock.getElapsedTime();
    }

    std::vector<std::string>&
    getDialog(std::string tag);
    int                 getFaceIndexByAvatar(int avatar) {
        if (avatar > 39) return -1;
        return m_avatarFace[avatar];
    }
    sf::Sprite&         getFaceByAvatar(int avatar)  {
        return m_faceset[m_avatarFace[avatar]];
    }
    sf::Sprite&         getFaceByIndex(int index)  {
        return m_faceset[index];
    }

    void                LoadBeacons(std::string level_name);
    void                LoadNpcs(std::string level_name);
    void                LoadWaypoints(std::string level_name);
    bool                LoadTriggers(std::string level);
    void                SaveBeacons(std::string level_name);
    void                SaveNpcs(std::string level_name);
    void                SaveWaypoints(std::string level_name);
    bool                SaveTriggers(std::string level_name);
    std::vector<Enemy>&         getEnemyRoster() {
        return m_EnemyRoster;
    }
    std::vector<sf::Sprite>&    getEnemyAvatars() {
        return m_EnemyAvatars;
    }
    std::vector<Trigger>&       getAllTriggers() {
        return m_Triggers;
    }
    std::vector<Trigger>        CurrentTriggerList;

private:
    bool                LoadDialogs(std::string level);
    bool                LoadSounds(std::string level);
    bool                LoadGraphics(std::string level);
    bool                LoadEnemyRoster(std::string level);
    void                DebugEnemyLoad(Enemy& enemy);

    void                PopulateFaceVector();
    void                PopulateTerrainSpriteVector();

    void                DebugDialogTree();

    /* Sounds */
    sf::SoundBuffer     m_Sound_StairBuffer;
    sf::Sound           m_Sound_Stairs;
    sf::SoundBuffer     m_Sound_SleepBuffer;
    sf::Sound           m_Sound_Sleep;
    sf::SoundBuffer     m_Sound_DoorBuffer;
    sf::Sound           m_Sound_Door;
    sf::SoundBuffer     m_Sound_DoorJiggleBuffer;
    sf::Sound           m_Sound_DoorJiggle;

    int                 m_levelChange = 0;
    int                 m_FadeAlpha = 0;
    bool                m_FadingIn = false;
    bool                m_FadingOut = false;
    bool                m_sleeping = false;
    std::string         name;
    std::string         newZoneList;
    std::string         oldZoneList;
    std::string         nz1, nz2, nz3, nz4;
    sf::Vector3i        prevLocation;

    sf::Time            m_prevNPCUpdateTime;
    float               m_NPCdelta;

    std::vector<Beacon> m_InactiveBeacons;
    std::vector<Beacon> m_CombatBeacons;
    std::vector<Beacon> m_SearchBeacons;
    std::vector<Beacon> m_LocationBeacons;

    std::map<std::string, std::vector<std::string>> m_Dialogs;
    const int           FACE_TOP            = 807;

    sf::Texture                 m_faces;
    std::vector<sf::Sprite>     m_faceset;
    std::vector<int>            m_avatarFace = { 1,  9, 19, 24, 10, 12,  7, 13, 23, 11, 18, 19,
                                         20, 22, 26, 11, 14, 24, 19, 17, 14, 23, 19,  8,
                                         16, -1, -1, -1, -1, -1, -1, -1, 20, 18, 26, -1,
                                         15,  0, -1, -1
                                       };
    std::vector<Enemy>             m_EnemyRoster;
    std::vector<sf::Sprite>        m_EnemyAvatars;
    sf::Texture                     m_EnemyAvatarTexture;
    std::vector<Trigger>           m_Triggers;
};

#endif // LEVEL_H
