#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <SFML/Graphics.hpp>
#include <bitset>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "map_tile.h"
#include "constants.h"
#include "map_zone.h"
#include "tool.h"
#include "boxes.h"
#include "gameengine.h"
#include "gamestate.h"
#include "states/edit_beacon.h"
#include "states/edit_objects.h"
#include "states/edit_triggers.h"
#include "states/input_string.h"

class StateEditMap : public GameState {
public:
    enum {SELECTOR_TINY, SELECTOR_SMALL, SELECTOR_MEDIUM, SELECTOR_LARGE, SELECTOR_HUGE};
    void Init(Engine* e);
    void Cleanup(Engine* e);
    void Pause(Engine* e);
    void Resume(Engine* e);
    void HandleEvents(Engine* e);
    void Update(Engine* e);
    void Draw(Engine* e);
    static StateEditMap* Instance() {
        return &m_StateEditMap;
    }

protected:
    StateEditMap() {}

private:
    void                        DesignerClickHandler(sf::Vector2i MouseClick, int button, int delta, Engine *e);
    void                        FillInFrames(int slot);
    void                        SaveOldFrames(int cell, Engine *e);
    void                        TileInspect(Tile& tile);
    void                        UpdateActiveTile(int CurrentTile, Engine* e);
    void                        TurnOffAnimDesigner();
    void                        TurnOnAnimDesigner();
    void                        DrawDesigner(sf::RenderWindow& window);
    void                        ScrollTileSheet(int delta);
    void                        ChangeSelectorSize(int delta);
    void                        LoadBeaconLocations(Engine* e);
    void                        floodFill(sf::Vector2f worldPos, Engine* e);
    void                        floodFillUtil(int (&screen)[64][40], int x, int y, int prevType, int newType);
    void                        ResetCell(int cell, Engine *e);
    void                        SetDuration(int& tenths);
    void                        SaveAnimatedTiles();
    void                        LoadAnimatedTiles(Engine* e);

    Boxes                       m_Boxes;
    static StateEditMap         m_StateEditMap;
    static sf::Vector2f         ConvertWorldPosToWorldTileCoordinate(sf::Vector2f pos);
    static sf::Vector2f         getWorldTilePos(sf::Vector2f worldPos);
    static void                 pv(sf::Vector2f pos, std::string s = "");
    static void                 pv(float x, float y, std::string s = "");
    Tile                        BuildTile(Engine* e, sf::Vector2f worldPos, int index, float duration = 0.0f, std::vector<int> animTiles = std::vector<int>());
    void                        DrawSelector(sf::RenderWindow& window);
    bool                        InsertTiles(Tile tile, Engine* e, sf::Vector2f pos, int dim);
    std::string                 Choice(int attr);
    Tile&                       GetTileAtWorldPos(sf::Vector2f worldPos, Engine* e);
    sf::Texture                 m_mapEditTexture;
    sf::Texture                 m_Overlay_Texture;
    sf::Texture                 m_TileSet;
    sf::Sprite                  m_Sprite_Selection;
    sf::Sprite                  m_Sprite_Tileset;

    sf::Vector2i                m_mousePos;
    sf::Vector2f                m_viewPos;
    sf::Vector2f                m_worldPos;

    sf::Time                    m_delta;
    bool                        bShowTiles = false;
    bool                        bShowAnimDesigner = false;
    int                         tileSheetY = 0;
    sf::Text                    Hud;
    std::stringstream           HudString;
    int                         CurrentTile;
    sf::RectangleShape          ShadedRectangle;
    sf::Vector2i                TilePick;
    Tile                        m_activeTile;
    int                         selectorSize = SELECTOR_SMALL;
    bool                        fillMode = false;
    bool                        beaconMode = false;

    int                         activeAnimCell = -1;
    int                         activeAnimFrameCell = 0;
    int                         tenths = 10;
    std::vector<Tile>           AnimatedTiles;
    std::vector<Tile>           AnimatedTileCells;
    std::map<int, sf::IntRect>  ClickMap;
    std::map<int, sf::IntRect>  ClickMapFrames;
    std::string                 m_InspectDetails;
    std::vector<sf::CircleShape> m_Beacons;
};

#endif
